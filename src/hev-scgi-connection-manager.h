/*
 ============================================================================
 Name        : hev-scgi-connection-manager.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_CONNECTION_MANAGER_H__
#define __HEV_SCGI_CONNECTION_MANAGER_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_CONNECTION_MANAGER	(hev_scgi_connection_manager_get_type())
#define HEV_SCGI_CONNECTION_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_CONNECTION_MANAGER, HevSCGIConnectionManager))
#define HEV_IS_SCGI_CONNECTION_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_CONNECTION_MANAGER))
#define HEV_SCGI_CONNECTION_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_CONNECTION_MANAGER, HevSCGIConnectionManagerClass))
#define HEV_IS_SCGI_CONNECTION_MANAGER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_CONNECTION_MANAGER))
#define HEV_SCGI_CONNECTION_MANAGER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_CONNECTION_MANAGER, HevSCGIConnectionManagerClass))

typedef struct _HevSCGIConnectionManager HevSCGIConnectionManager;
typedef struct _HevSCGIConnectionManagerClass HevSCGIConnectionManagerClass;

struct _HevSCGIConnectionManager
{
	GObject parent_instance;
};

struct _HevSCGIConnectionManagerClass
{
	GObjectClass parent_class;

	void (*new_task)(HevSCGIConnectionManager *self, GObject *task);
};

GType hev_scgi_connection_manager_get_type(void);

GObject * hev_scgi_connection_manager_new(void);
void hev_scgi_connection_manager_take_over(HevSCGIConnectionManager *self,
			GSocketConnection *connection);

G_END_DECLS

#endif /* __HEV_SCGI_CONNECTION_MANAGER_H__ */

