/*
 ============================================================================
 Name        : hev-scgi-task.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_TASK_H__
#define __HEV_SCGI_TASK_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_TASK	(hev_scgi_task_get_type())
#define HEV_SCGI_TASK(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_TASK, HevSCGITask))
#define HEV_IS_SCGI_TASK(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_TASK))
#define HEV_SCGI_TASK_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_TASK, HevSCGITaskClass))
#define HEV_IS_SCGI_TASK_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_TASK))
#define HEV_SCGI_TASK_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_TASK, HevSCGITaskClass))

typedef struct _HevSCGITask HevSCGITask;
typedef struct _HevSCGITaskClass HevSCGITaskClass;

struct _HevSCGITask
{
	GObject parent_instance;
};

struct _HevSCGITaskClass
{
	GObjectClass parent_class;
};

GType hev_scgi_task_get_type(void);

GObject * hev_scgi_task_new(void);
GObject * hev_scgi_task_get_request(HevSCGITask *self);
GObject * hev_scgi_task_get_response(HevSCGITask *self);
void hev_scgi_task_set_socket_connection(HevSCGITask *self,
			GObject *connection);
void hev_scgi_task_set_handler(HevSCGITask *self, GObject *handler);
GObject * hev_scgi_task_get_handler(HevSCGITask *self);

G_END_DECLS

#endif /* __HEV_SCGI_TASK_H__ */

