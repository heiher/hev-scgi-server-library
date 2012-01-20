/*
 ============================================================================
 Name        : hev-scgi-server.h
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#ifndef __HEV_SCGI_SERVER_H__
#define __HEV_SCGI_SERVER_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEV_TYPE_SCGI_SERVER	(hev_scgi_server_get_type())
#define HEV_SCGI_SERVER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), HEV_TYPE_SCGI_SERVER, HevSCGIServer))
#define HEV_IS_SCGI_SERVER(obj)	(G_TYPE_CHECK_INSTANCE_TYPE((obj), HEV_TYPE_SCGI_SERVER))
#define HEV_SCGI_SERVER_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST((klass), HEV_TYPE_SCGI_SERVER, HevSCGIServerClass))
#define HEV_IS_SCGI_SERVER_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE((klass), HEV_TYPE_SCGI_SERVER))
#define HEV_SCGI_SERVER_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS((obj), HEV_TYPE_SCGI_SERVER, HevSCGIServerClass))

typedef struct _HevSCGIServer HevSCGIServer;
typedef struct _HevSCGIServerClass HevSCGIServerClass;

struct _HevSCGIServer
{
	GObject parent_instance;
};

struct _HevSCGIServerClass
{
	GObjectClass parent_class;
};

GType hev_scgi_server_get_type(void);

GObject * hev_scgi_server_new(void);
void hev_scgi_server_start(HevSCGIServer *self);
void hev_scgi_server_stop(HevSCGIServer *self);

G_END_DECLS

#endif /* __HEV_SCGI_SERVER_H__ */

