/*
 ============================================================================
 Name        : hev-scgi-connection-manager.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-scgi-connection-manager.h"

#define HEV_SCGI_CONNECTION_MANAGER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_CONNECTION_MANAGER, HevSCGIConnectionManagerPrivate))

typedef struct _HevSCGIConnectionManagerPrivate HevSCGIConnectionManagerPrivate;

struct _HevSCGIConnectionManagerPrivate
{
	gchar c;
};

G_DEFINE_TYPE(HevSCGIConnectionManager, hev_scgi_connection_manager, G_TYPE_SOCKET_SERVICE);

static void hev_scgi_connection_manager_dispose(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_connection_manager_parent_class)->dispose(obj);
}

static void hev_scgi_connection_manager_finalize(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_connection_manager_parent_class)->finalize(obj);
}

static GObject * hev_scgi_connection_manager_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return G_OBJECT_CLASS(hev_scgi_connection_manager_parent_class)->constructor(type, n, param);
}

static void hev_scgi_connection_manager_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_connection_manager_parent_class)->constructed(obj);
}

static void hev_scgi_connection_manager_class_init(HevSCGIConnectionManagerClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_connection_manager_constructor;
	obj_class->constructed = hev_scgi_connection_manager_constructed;
	obj_class->dispose = hev_scgi_connection_manager_dispose;
	obj_class->finalize = hev_scgi_connection_manager_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGIConnectionManagerPrivate));
}

static void hev_scgi_connection_manager_init(HevSCGIConnectionManager * self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject * hev_scgi_connection_manager_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	return g_object_new(HEV_TYPE_SCGI_CONNECTION_MANAGER, NULL);
}

