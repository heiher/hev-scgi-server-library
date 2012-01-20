/*
 ============================================================================
 Name        : hev-scgi-task-dispatcher.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_TASK_DISPATCHER_H__
#define __HEV_SCGI_TASK_DISPATCHER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_TASK_DISPATCHER	(hev_scgi_task_dispatcher_get_type())
#define HEV_SCGI_TASK_DISPATCHER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_TASK_DISPATCHER, HevSCGITaskDispatcher))
#define HEV_IS_SCGI_TASK_DISPATCHER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_TASK_DISPATCHER))
#define HEV_SCGI_TASK_DISPATCHER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_TASK_DISPATCHER, HevSCGITaskDispatcherClass))
#define HEV_IS_SCGI_TASK_DISPATCHER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_TASK_DISPATCHER))
#define HEV_SCGI_TASK_DISPATCHER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_TASK_DISPATCHER, HevSCGITaskDispatcherClass))

typedef struct _HevSCGITaskDispatcher HevSCGITaskDispatcher;
typedef struct _HevSCGITaskDispatcherClass HevSCGITaskDispatcherClass;

struct _HevSCGITaskDispatcher
{
	GObject parent_instance;
};

struct _HevSCGITaskDispatcherClass
{
	GObjectClass parent_class;
};

GType hev_scgi_task_dispatcher_get_type(void);

GObject * hev_scgi_task_dispatcher_new(void);
void hev_scgi_task_dispatcher_push(HevSCGITaskDispatcher *self,
			GObject *scgi_task);
void hev_scgi_task_dispatcher_add_handler(HevSCGITaskDispatcher *self,
			GObject *scgi_handler);

G_END_DECLS

#endif /* __HEV_SCGI_TASK_DISPATCHER_H__ */

