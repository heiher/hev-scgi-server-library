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
#include "hev-scgi-task.h"
#include "hev-scgi-request.h"
#include "hev-scgi-response.h"

enum
{
	NEW_TASK,
	LAST_SIGNAL
};

static guint hev_scgi_connection_manager_signals[LAST_SIGNAL] = { 0 };

static void hev_scgi_connection_manager_real_new_task(HevSCGIConnectionManager *self,
			GObject *scgi_task);

#define HEV_SCGI_CONNECTION_MANAGER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_CONNECTION_MANAGER, HevSCGIConnectionManagerPrivate))

typedef struct _HevSCGIConnectionManagerPrivate HevSCGIConnectionManagerPrivate;

struct _HevSCGIConnectionManagerPrivate
{
	gchar c;
};

G_DEFINE_TYPE(HevSCGIConnectionManager, hev_scgi_connection_manager, G_TYPE_OBJECT);

static void hev_scgi_connection_manager_dispose(GObject * obj)
{
	HevSCGIConnectionManager * self = HEV_SCGI_CONNECTION_MANAGER(obj);
	HevSCGIConnectionManagerPrivate * priv = HEV_SCGI_CONNECTION_MANAGER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_connection_manager_parent_class)->dispose(obj);
}

static void hev_scgi_connection_manager_finalize(GObject * obj)
{
	HevSCGIConnectionManager * self = HEV_SCGI_CONNECTION_MANAGER(obj);
	HevSCGIConnectionManagerPrivate * priv = HEV_SCGI_CONNECTION_MANAGER_GET_PRIVATE(self);

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
}

static void hev_scgi_connection_manager_class_init(HevSCGIConnectionManagerClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_connection_manager_constructor;
	obj_class->constructed = hev_scgi_connection_manager_constructed;
	obj_class->dispose = hev_scgi_connection_manager_dispose;
	obj_class->finalize = hev_scgi_connection_manager_finalize;

	klass->new_task = hev_scgi_connection_manager_real_new_task;

	hev_scgi_connection_manager_signals[NEW_TASK] =
		g_signal_new("new_task", G_TYPE_FROM_CLASS(klass),
					G_SIGNAL_RUN_LAST,
					G_STRUCT_OFFSET(HevSCGIConnectionManagerClass, new_task),
					NULL, NULL, g_cclosure_marshal_VOID__OBJECT,
					G_TYPE_NONE, 1, G_TYPE_OBJECT);

	g_type_class_add_private(klass, sizeof(HevSCGIConnectionManagerPrivate));
}

static void hev_scgi_connection_manager_init(HevSCGIConnectionManager * self)
{
	HevSCGIConnectionManagerPrivate * priv = HEV_SCGI_CONNECTION_MANAGER_GET_PRIVATE(self);
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject * hev_scgi_connection_manager_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_CONNECTION_MANAGER, NULL);
}

void hev_scgi_connection_manager_take_over(HevSCGIConnectionManager *self,
			GSocketConnection *connection)
{
	HevSCGIConnectionManagerPrivate *priv = NULL;
	GObject *scgi_task = NULL;
	GObject *scgi_request = NULL;
	GObject *scgi_response = NULL;
	GInputStream *input_stream = NULL;
	GOutputStream *output_stream = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_return_if_fail(HEV_IS_SCGI_CONNECTION_MANAGER(self));
	priv = HEV_SCGI_CONNECTION_MANAGER_GET_PRIVATE(self);

	scgi_task = hev_scgi_task_new();
	if(!scgi_task)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	hev_scgi_task_set_socket_connection(HEV_SCGI_TASK(scgi_task),
				G_OBJECT(connection));

	scgi_request = hev_scgi_task_get_request(HEV_SCGI_TASK(scgi_task));
	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));

	input_stream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
	output_stream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

	hev_scgi_request_set_input_stream(HEV_SCGI_REQUEST(scgi_request),
				input_stream);
	hev_scgi_response_set_output_stream(HEV_SCGI_RESPONSE(scgi_response),
				output_stream);

	g_signal_emit(G_OBJECT(self), hev_scgi_connection_manager_signals[NEW_TASK],
				0, scgi_task);
}

static void hev_scgi_connection_manager_real_new_task(HevSCGIConnectionManager *self,
			GObject *scgi_task)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_object_unref(scgi_task);
}

