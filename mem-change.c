#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/sched/mm.h>
#include <linux/mm.h>
#include <linux/pid.h>

#include <linux/uio.h>
#include <linux/pagemap.h>
#include <linux/vmalloc.h>
#include <linux/highmem.h>
#include <linux/ptrace.h>
#include <linux/syscalls.h>

char pattern[] = "85 c0 75 f9";
char replace[] = {0x90, 0x90, 0x90, 0x90}; /* Replace loop with NOP slide */
const char proc_name[] = "loop";

void *find_pattern(char *buffer, char *pattern, size_t buffer_size, size_t pattern_size)
{
	size_t i;

	pattern_size = pattern_size/3;
	buffer_size = buffer_size - pattern_size;

	for(i = 0; i < buffer_size; i++)
	{
		size_t i2 = i;
		char *patt_byte = pattern;

		while(i2 < buffer_size)
		{
			char byte;

			if(!strncmp(patt_byte, "??", 2))
			{
				i2++;
				patt_byte = strchr(patt_byte, ' ');
				if(patt_byte == NULL)
					return &buffer[i];

				patt_byte++;
				continue;
			}

			sscanf(patt_byte, "%hhx", &byte);

			if(buffer[i2] != byte)
				break;

			patt_byte = strchr(patt_byte, ' ');
			if(patt_byte == NULL)
				return &buffer[i];

			patt_byte++;
			i2++;
		}
	}

	return NULL;
}

static int mem_change(void)
{
	struct task_struct *task;

	for_each_process(task)
	{
		if(!strcmp(task->comm, proc_name))
		{
			struct vm_area_struct *current_mmap;
			struct page *page;
			int locked = 1;

			pr_info("Found process with PID:[%d]\n", task->pid);

			for(current_mmap = task->mm->mmap; (current_mmap = current_mmap->vm_next) != NULL;)
			{
				if(current_mmap->vm_file == NULL)
					continue;

				if(strcmp(current_mmap->vm_file->f_path.dentry->d_iname, proc_name))
					continue;

				down_write(&task->mm->mmap_lock);

				if(get_user_pages_remote(task->mm,
							current_mmap->vm_start, 1, FOLL_FORCE, &page, NULL, &locked) == 1)

				{
					void *target = kmap(page);
					size_t i = 0;
					void *ret = NULL;
					/* TODO: check for memset out of bounds write */

					pr_info("Searching for pattern...\n");
					ret = find_pattern(target, pattern, PAGE_SIZE, sizeof(pattern));
					if(ret == NULL)
					{
						kunmap(page);
						up_write(&task->mm->mmap_lock);
						continue;
					}
					pr_info("Pattern found!\n");

					for(; i < sizeof(replace); i++)
					{
						memset(ret++, replace[i], 1);
					}

					kunmap(page);

				}

				up_write(&task->mm->mmap_lock);
				break;
			}
		}
	}

	return 0;
}

static void mod_exit(void)
{
	pr_info("Unloading module\n");
}

MODULE_LICENSE("GPL");
module_init(mem_change);
module_exit(mod_exit);
