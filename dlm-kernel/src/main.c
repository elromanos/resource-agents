/******************************************************************************
*******************************************************************************
**
**  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
**  Copyright (C) 2004 Red Hat, Inc.  All rights reserved.
**  
**  This copyrighted material is made available to anyone wishing to use,
**  modify, copy, or redistribute it subject to the terms and conditions
**  of the GNU General Public License v.2.
**
*******************************************************************************
******************************************************************************/

#define EXPORT_SYMTAB

#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <linux/module.h>
#include <net/sock.h>

#include <cluster/cnxman.h>

#include "dlm_internal.h"
#include "lockspace.h"
#include "ast.h"
#include "lkb.h"
#include "nodes.h"
#include "locking.h"
#include "config.h"
#include "memory.h"
#include "recover.h"
#include "lowcomms.h"

int  dlm_device_init(void);
void dlm_device_exit(void);
void dlm_proc_init(void);
void dlm_proc_exit(void);


/* Cluster manager callbacks, we want to know if a node dies
   N.B. this is independent of lockspace-specific event callbacks from SM */

static void cman_callback(kcl_callback_reason reason, long arg)
{
	if (reason == DIED) {
		lowcomms_close((int) arg);
	}

	/* This is unconditional. so do what we can to tidy up */
	if (reason == LEAVING) {
	        dlm_emergency_shutdown();
	}
}

int __init init_dlm(void)
{
	dlm_proc_init();
	dlm_lockspace_init();
	dlm_nodes_init();
	dlm_device_init();
	dlm_memory_init();
	dlm_config_init();

	kcl_add_callback(cman_callback);

	printk("DLM %s (built %s %s) installed\n",
	       DLM_RELEASE_NAME, __DATE__, __TIME__);

	return 0;
}

void __exit exit_dlm(void)
{
	kcl_remove_callback(cman_callback);

	dlm_device_exit();
	dlm_memory_exit();
	dlm_config_exit();
	dlm_proc_exit();
}

MODULE_DESCRIPTION("Distributed Lock Manager " DLM_RELEASE_NAME);
MODULE_AUTHOR("Red Hat, Inc.");
MODULE_LICENSE("GPL");

module_init(init_dlm);
module_exit(exit_dlm);

EXPORT_SYMBOL(dlm_init);
EXPORT_SYMBOL(dlm_release);
EXPORT_SYMBOL(dlm_new_lockspace);
EXPORT_SYMBOL(dlm_release_lockspace);
EXPORT_SYMBOL(dlm_lock);
EXPORT_SYMBOL(dlm_unlock);
EXPORT_SYMBOL(dlm_debug_dump);
EXPORT_SYMBOL(dlm_locks_dump);
