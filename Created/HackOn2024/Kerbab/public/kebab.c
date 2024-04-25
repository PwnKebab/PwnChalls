#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PwnKebab");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("LKM Vulnerable Safe Guard");
 
#define DEVICE_NAME "safe_guard"
#define KEBAB_MAX_BUFFER_SIZE 2048
#define KEBAB_IOCTL_NEW       0xFABADA
#define KEBAB_IOCTL_READ      0xBEBE
#define KEBAB_IOCTL_SET_KEY   0x1CAFE
#define KEBAB_MAX_BUFFERS     4
#define MAX_RC4_LEN           256

struct secure_buffer {
	char *buffer;
	size_t size;
};

struct new_secbuff_arg {
	size_t size;
	char key[MAX_RC4_LEN];
	const char *buffer;
};

struct read_secbuff_arg {
	unsigned long index;
	char key[MAX_RC4_LEN];
	char *buffer;
};

struct key_info{
	int pid;
	struct task_struct *cur;
	size_t max_len;
};

static struct secure_buffer *sec_buffs[KEBAB_MAX_BUFFERS] = {0};
static unsigned char n_buffs = 0;
static struct mutex kebab_mutex;
static char RC4_key[MAX_RC4_LEN + 1] = {0};

struct rc4_state {
	unsigned char	perm[256];
	unsigned char	index1;
	unsigned char	index2;
};

static __inline void swap_bytes(unsigned char *a, unsigned char *b)
{
	unsigned char temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void rc4_init(struct rc4_state *const state, unsigned char *key, int keylen)
{
	unsigned char j;
	int i;

	for (i = 0; i < 256; i++)
		state->perm[i] = (unsigned char)i; 
	state->index1 = 0;
	state->index2 = 0;

	for (j = i = 0; i < 256; i++) {
		j += state->perm[i] + key[i % keylen]; 
		swap_bytes(&state->perm[i], &state->perm[j]);
	}
}

void rc4_crypt(struct rc4_state *const state, const unsigned char *inbuf, unsigned char *outbuf, int buflen)
{
	int i;
	unsigned char j;

	for (i = 0; i <= buflen; i++) {

		state->index1++;
		state->index2 += state->perm[state->index1];

		swap_bytes(&state->perm[state->index1],
		    &state->perm[state->index2]);

		j = state->perm[state->index1] + state->perm[state->index2];
		outbuf[i] = inbuf[i] ^ state->perm[j];
	}
}

int RC4(char *user_buff, unsigned char *sec_buff, size_t size)
{
	struct rc4_state rc4st;
	rc4_init(&rc4st, RC4_key, strlen(RC4_key));
	rc4_crypt(&rc4st, user_buff, sec_buff, size);

	return 0;
}

static int is_the_same_key(char *key)
{
	return !strncmp(key, RC4_key, MAX_RC4_LEN);
}

static int init_secure_buff(struct new_secbuff_arg *arg_struct)
{
	char *u_buffer;

	u_buffer = kzalloc(KEBAB_MAX_BUFFER_SIZE, GFP_KERNEL);

	if (!u_buffer) return -EINVAL;

	if (copy_from_user(u_buffer, arg_struct->buffer, arg_struct->size)) return -EFAULT;

	RC4(u_buffer, sec_buffs[n_buffs-1]->buffer, arg_struct->size);

	kfree(u_buffer);
	
	return 0;
}

static int alloc_secure_buff(struct new_secbuff_arg *arg_struct)
{

    sec_buffs[n_buffs] = kmalloc(sizeof(struct secure_buffer), GFP_KERNEL);
	if (!sec_buffs[n_buffs]) return -EINVAL;

	sec_buffs[n_buffs]->buffer = kzalloc(arg_struct->size, GFP_KERNEL);

	if (!sec_buffs[n_buffs]->buffer)
	{
		kfree(sec_buffs[n_buffs]);
		sec_buffs[n_buffs] = 0;
		return -EINVAL;
	}

	sec_buffs[n_buffs]->size = arg_struct->size;

	printk(KERN_INFO "kebab: created %d\n", n_buffs);
    n_buffs++;
	return 0;
}

static int ioctl_new(void __user *argp)
{

	struct new_secbuff_arg arg_struct;

	if (copy_from_user(&arg_struct, argp, sizeof(arg_struct))) return -EFAULT;

	if(!is_the_same_key(arg_struct.key)) return -EINVAL;

	if (arg_struct.size > KEBAB_MAX_BUFFER_SIZE) return -EFAULT;

	if (n_buffs >= KEBAB_MAX_BUFFERS) return -ENOMEM;

    if (alloc_secure_buff(&arg_struct) == -EINVAL) return -EINVAL;

	return init_secure_buff(&arg_struct);
}

static int ioctl_read(void __user *argp)
{
	struct read_secbuff_arg arg_struct;
 
	if (copy_from_user(&arg_struct, argp, sizeof arg_struct)) return -EFAULT;

	if(!is_the_same_key(arg_struct.key)) return -EINVAL;

	if (arg_struct.index >= n_buffs) return -EINVAL;
 
	if (copy_to_user(arg_struct.buffer, sec_buffs[arg_struct.index]->buffer, sec_buffs[arg_struct.index]->size)) return -EFAULT;
 
	printk(KERN_INFO "kebab: read %zu bytes from %lu\n", sec_buffs[arg_struct.index]->size, arg_struct.index);
	return 0;
}


static int ioctl_set_key(void __user *argp)
{
	if (copy_from_user(&RC4_key, argp, MAX_RC4_LEN)) return -EFAULT;
	struct key_info info = {current->pid, current, MAX_RC4_LEN};

	if (copy_to_user(argp, &info, sizeof info)) return -EFAULT;
	printk(KERN_INFO "kebab: key setted\n");
	return 0;
}

static long kebab_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	void __user *argp = (void __user *) arg;
	int err = 0;

	if (!mutex_trylock(&kebab_mutex)) return -EAGAIN;

	switch (cmd)
	{
        case KEBAB_IOCTL_NEW:
			if (strlen(RC4_key) == 0)
			{
				err = -EINVAL;
				break;
			}
            err = ioctl_new(argp);
            break;
        case KEBAB_IOCTL_READ:
			if (strlen(RC4_key) == 0)
			{
				err = -EINVAL;
				break;
			}
            err = ioctl_read(argp);
            break;
		case KEBAB_IOCTL_SET_KEY:
			if (strlen(RC4_key) > 0)
			{
				err = -EINVAL;
				break;
			}
			err = ioctl_set_key(argp);
			break;
        default:
            err = -EINVAL;
	}

	mutex_unlock(&kebab_mutex);
	return err;
}

static int kebab_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int kebab_release(struct inode *inode, struct file *file)
{
	return 0;
}

static const struct file_operations kebab_fops = {
	.owner = THIS_MODULE,
	.open = kebab_open,
	.release = kebab_release,
	.unlocked_ioctl = kebab_ioctl,
};
 
static struct miscdevice kebab_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &kebab_fops,
	.mode = 0600
};

static int __init kebab_init(void)
{
	int err;
 
	mutex_init(&kebab_mutex);
 
	err = misc_register(&kebab_device);
	if (err < 0)
	{
		printk(KERN_ALERT "kebab: failed to misc_register\n");
		return err;
	}
 
	printk(KERN_INFO "kebab: module initialized\n");
	return 0;
}

static void _destroy_secure_buffer(int idx)
{
	if (sec_buffs[idx] != 0)
	{
		kfree(sec_buffs[idx]->buffer);
		kfree(sec_buffs[idx]);
		sec_buffs[idx] = NULL;
	}
}

static void __exit kebab_exit(void)
{
	int i;
 
	for (i = 0; i < KEBAB_MAX_BUFFERS; i++)
	{
		_destroy_secure_buffer(i);
	}
 
	misc_deregister(&kebab_device);
	printk(KERN_INFO "kebab: module exited\n");
}
 
module_init(kebab_init);
module_exit(kebab_exit);
