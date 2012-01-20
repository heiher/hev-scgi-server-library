/*
 ============================================================================
 Name        : hev-scgi-task.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-scgi-task.h"
#include "hev-scgi-request.h"
#include "hev-scgi-response.h"

#define HEV_SCGI_TASK_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_TASK, HevSCGITaskPrivate))

typedef struct _HevSCGITaskPrivate HevSCGITaskPrivate;

struct _HevSCGITaskPrivate
{
	GObject *scgi_request;
	GObject *scgi_response;
	GObject *connection;
	GObject *handler;
};

G_DEFINE_TYPE(HevSCGITask, hev_scgi_task, G_TYPE_OBJECT);

static void hev_scgi_task_dispose(GObject * obj)
{
	HevSCGITask * self = HEV_SCGI_TASK(obj);
	HevSCGITaskPrivate * priv = HEV_SCGI_TASK_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->scgi_request)
	{
		g_object_unref(priv->scgi_request);
		priv->scgi_request = NULL;
	}

	if(priv->scgi_response)
	{
		g_object_unref(priv->scgi_response);
		priv->scgi_response = NULL;
	}

	if(priv->connection)
	{
		g_object_unref(priv->connection);
		priv->connection = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_task_parent_class)->dispose(obj);
}

static void hev_scgi_task_finalize(GObject * obj)
{
	HevSCGITask * self = HEV_SCGI_TASK(obj);
	HevSCGITaskPrivate * priv = HEV_SCGI_TASK_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_task_parent_class)->finalize(obj);
}

static GObject * hev_scgi_task_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_task_parent_class)->constructor(type, n, param);
}

static void hev_scgi_task_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_scgi_task_class_init(HevSCGITaskClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_task_constructor;
	obj_class->constructed = hev_scgi_task_constructed;
	obj_class->dispose = hev_scgi_task_dispose;
	obj_class->finalize = hev_scgi_task_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGITaskPrivate));
}

static void hev_scgi_task_init(HevSCGITask * self)
{
	HevSCGITaskPrivate * priv = HEV_SCGI_TASK_GET_PRIVATE(self);
	
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	
	priv->scgi_request = hev_scgi_request_new();
	if(!priv->scgi_request)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->scgi_response = hev_scgi_response_new();
	if(!priv->scgi_response)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject * hev_scgi_task_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_TASK, NULL);
}

GObject * hev_scgi_task_get_request(HevSCGITask *self)
{
	HevSCGITaskPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_TASK(self), NULL);
	priv = HEV_SCGI_TASK_GET_PRIVATE(self);

	return priv->scgi_request;
}

GObject * hev_scgi_task_get_response(HevSCGITask *self)
{
	HevSCGITaskPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_val_if_fail(HEV_IS_SCGI_TASK(self), NULL);
	priv = HEV_SCGI_TASK_GET_PRIVATE(self);

	return priv->scgi_response;
}

void hev_scgi_task_set_socket_connection(HevSCGITask *self,
			GObject *connection)
{
	HevSCGITaskPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(HEV_IS_SCGI_TASK(self));
	g_return_if_fail(G_IS_OBJECT(connection));
	priv = HEV_SCGI_TASK_GET_PRIVATE(self);

	if(priv->connection)
	  g_object_unref(priv->connection);

	priv->connection = g_object_ref(connection);
}

void hev_scgi_task_set_handler(HevSCGITask *self, GObject *handler)
{
	HevSCGITaskPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(HEV_IS_SCGI_TASK(self));
	g_return_if_fail(G_IS_OBJECT(handler));
	priv = HEV_SCGI_TASK_GET_PRIVATE(self);

	priv->handler = handler;
}

GObject * hev_scgi_task_get_handler(HevSCGITask *self)
{
	HevSCGITaskPrivate *priv = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(HEV_IS_SCGI_TASK(self));
	priv = HEV_SCGI_TASK_GET_PRIVATE(self);

	return priv->handler;
}

