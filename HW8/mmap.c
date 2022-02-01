//#include <linux/config.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

#ifdef MODVERSIONS
#  include <linux/modversions.h>
#endif
#include <asm/io.h>

/* character device structures */
static dev_t mmap_dev;
static struct cdev mmap_cdev;

/* methods of the character device */
static int mmap_open(struct inode *inode, struct file *filp);
static int mmap_release(struct inode *inode, struct file *filp);
static int mmap_mmap(struct file *filp, struct vm_area_struct *vma);
static vm_fault_t vm_fault(struct vm_fault *vmf);
static void vm_close(struct vm_area_struct *vma);
static void vm_open(struct vm_area_struct *vma);

/* the file operations, i.e. all character device methods */
static struct file_operations mmap_fops = {
        .open = mmap_open,
        .release = mmap_release,
        .mmap = mmap_mmap,
        .owner = THIS_MODULE,
};


static struct vm_operations_struct vm_ops =
{
    .close = vm_close,
    .fault = vm_fault,
    .open = vm_open,
   
};


// internal data
// length of the two memory areas
#define NPAGES 16
// pointer to the vmalloc'd area - alway page aligned
static int *vmalloc_area;
// pointer to the kmalloc'd area, rounded up to a page boundary
static int *kmalloc_area;
// original pointer for kmalloc'd area as returned by kmalloc
static void *kmalloc_ptr;

static vm_fault_t vm_fault(struct vm_fault *vmf)
{
	 /*
	 //all actions : allocated in virtual address space - >  virt_to_phys converts kernel virtual addresses into physical addresses-> 
	 obtain page frame number corresponded to physical address -> return page
	 
	 
	 vmf->pgoff - offset wich was used by user for access to the relevant page(to or from which page user want to start mapping, correspond to "offset" argument in usersapce mmap
	 */
	 printk("vmf->vma->vm_pgoff - %ld\n",vmf->vma->vm_pgoff);
	 printk("vmf->pgoff-%ld\n",vmf->pgoff);
	 
        if (vmf->vma->vm_pgoff == NPAGES) {
		 //vmf->page = pfn_to_page(virt_to_phys((void *)kmalloc_area) >> PAGE_SHIFT );//previous version - always read the same addresses, issue to fix:  move in kmalloc area from page to page
		 vmf->page = virt_to_page((char *)kmalloc_area + PAGE_SIZE * (vmf->pgoff - NPAGES));
		 get_page(vmf->page); //fault handlers should return a  page here,
		 pr_warn("kmalloc page fault handler executed\n");
        }
        
        if (vmf->vma->vm_pgoff == 0) {
		vmf->page = vmalloc_to_page((char *)vmalloc_area + PAGE_SIZE * vmf->pgoff);
		get_page(vmf->page); 
		pr_warn("vmalloc page fault handler executed\n");
                
        }
        
	return 0;
}
static void vm_close(struct vm_area_struct *vma)
{
    pr_info("vm_close\n");
}
static void vm_open(struct vm_area_struct *vma)
{
    pr_info("vm_open\n");
}

static int mmap_open(struct inode *inode, struct file *filp)
{
        return 0;
}
/* character device last close method */
static int mmap_release(struct inode *inode, struct file *filp)
{
        return 0;
}

// helper function, mmap's the kmalloc'd area which is physically contiguous
int mmap_kmem(struct file *filp, struct vm_area_struct *vma)
{
        int ret;
        long length = vma->vm_end - vma->vm_start;
	 vma->vm_ops = &vm_ops;
        /* check length - do not allow larger mappings than the number of
           pages allocated */
        if (length > NPAGES * PAGE_SIZE)
                return -EIO;

        /* will map a contiguous physical address space into the virtual space represented by vm_area_struct)*/
       /*  if ((ret = remap_pfn_range(vma,
                                   vma->vm_start,
                                   virt_to_phys((void *)kmalloc_area) >> PAGE_SHIFT,
                                   length,
                                   vma->vm_page_prot)) < 0) {
             
                return ret;
        }*/
        return 0;
}
// helper function, mmap's the vmalloc'd area which is not physically contiguous
int mmap_vmem(struct file *filp, struct vm_area_struct *vma)
{
        int ret;
        long length = vma->vm_end - vma->vm_start;
        unsigned long start = vma->vm_start;
        char *vmalloc_area_ptr = (char *)vmalloc_area;
        unsigned long pfn;
        vma->vm_ops = &vm_ops;

        /* check length - do not allow larger mappings than the number of
           pages allocated */
        if (length > NPAGES * PAGE_SIZE)
                return -EIO;

        /* loop over all pages, map it page individually */
          /*  while (length > 0) {
                pfn = vmalloc_to_pfn(vmalloc_area_ptr);
               if ((ret = remap_pfn_range(vma, start, pfn, PAGE_SIZE,
                                           PAGE_SHARED)) < 0) {
                        return ret;
                }
                start += PAGE_SIZE;
                vmalloc_area_ptr += PAGE_SIZE;
                length -= PAGE_SIZE;
        }*/
          //pr_info("vmalloc start %lx\n",vma->vm_start);
         // pr_info("vmalloc end %lx\n",vma->vm_end);
        return 0;
}

/* character device mmap method */
static int mmap_mmap(struct file *filp, struct vm_area_struct *vma)
{
        /* at offset 0 we map the vmalloc'd area */

         if (vma->vm_pgoff == 0) {
                return mmap_vmem(filp, vma);
                
        }
        /* at offset NPAGES we map the kmalloc'd area */
        if (vma->vm_pgoff == NPAGES) {
                return mmap_kmem(filp, vma);
        }
        /* at any other offset we return an error */
        return -EIO;
}

/* module initialization - called at module load time */
static int __init mymmap_init(void)
{
        int ret = 0;
        int i;

        /* allocate a memory area with kmalloc. Will be rounded up to a page boundary */
        if ((kmalloc_ptr = kmalloc((NPAGES + 2) * PAGE_SIZE, GFP_KERNEL)) == NULL) {
                ret = -ENOMEM;
                goto out;
        }
        /* round it up to the page bondary */
        kmalloc_area = (int *)((((unsigned long)kmalloc_ptr) + PAGE_SIZE - 1) & PAGE_MASK);

        /* allocate a memory area with vmalloc. */
        if ((vmalloc_area = (int *)vmalloc(NPAGES * PAGE_SIZE)) == NULL) {
                ret = -ENOMEM;
                goto out_kfree;
        }

        /* get the major number of the character device */
        if ((ret = alloc_chrdev_region(&mmap_dev, 0, 1, "mmap")) < 0) {
                printk(KERN_ERR "could not allocate major number for mmap\n");
                goto out_vfree;
        }
	
        /* initialize the device structure and register the device with the kernel */
        cdev_init(&mmap_cdev, &mmap_fops);
        if ((ret = cdev_add(&mmap_cdev, mmap_dev, 1)) < 0) {
                printk(KERN_ERR "could not allocate chrdev for mmap\n");
                goto out_unalloc_region;
        }
        pr_info("%d\n", MAJOR(mmap_dev));

        /* mark the pages reserved */
        for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE) {
                //SetPageReserved(vmalloc_to_page((void *)(((unsigned long)vmalloc_area) + i)));
                SetPageReserved(virt_to_page(((unsigned long)kmalloc_area) + i));
        }

        /* store a pattern in the memory - the test application will check for it */
        for (i = 0; i < (NPAGES * PAGE_SIZE / sizeof(int)); i += 2) {
                vmalloc_area[i] = (0xaffe << 16) + i;
                vmalloc_area[i + 1] = (0xbeef << 16) + i;
                kmalloc_area[i] = (0xdead << 16) + i;
                kmalloc_area[i + 1] = (0xbeef << 16) + i;
        }
        
     return ret;
        
  out_unalloc_region:
        unregister_chrdev_region(mmap_dev, 1);
  out_vfree:
        vfree(vmalloc_area);
  out_kfree:
        kfree(kmalloc_ptr);
  out:
        return ret;
}

/* module unload */
static void __exit mmap_exit(void)
{
        int i;

        /* remove the character deivce */
        cdev_del(&mmap_cdev);
        unregister_chrdev_region(mmap_dev, 1);

        /* unreserve the pages */
        for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE) {
                ClearPageReserved(vmalloc_to_page((void *)(((unsigned long)vmalloc_area) + i)));
                ClearPageReserved(virt_to_page(((unsigned long)kmalloc_area) + i));
        }
        /* free the memory areas */
        vfree(vmalloc_area);
        kfree(kmalloc_ptr);
        pr_info("module deinited\n");
}

module_init(mymmap_init);
module_exit(mmap_exit);
MODULE_DESCRIPTION("mmap demo driver");
MODULE_AUTHOR("Martin Frey <frey@scs.ch>");
MODULE_LICENSE("Dual BSD/GPL");

