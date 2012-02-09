/*
 ============================================================================
 Name        : hev-scgi-server.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include <gio/gio.h>

#include "hev-scgi-server.h"
#include "hev-scgi-config.h"
#include "hev-scgi-connection-manager.h"
#include "hev-scgi-task-dispatcher.h"
#include "hev-scgi-handler-module.h"
#include "hev-scgi-handler-default.h"

static gboolean socket_service_incoming_handler(GSocketService *service,
			GSocketConnection *connection, GObject *source_object,
			gpointer user_data);
static void scgi_connection_manager_new_task_handler(HevSCGIConnectionManager *connection_manager,
			GObject *scgi_task, gpointer user_data);

#define HEV_SCGI_SERVER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_SERVER, HevSCGIServerPrivate))

typedef struct _HevSCGIServerPrivate HevSCGIServerPrivate;

struct _HevSCGIServerPrivate
{
	GObject *scgi_config;
	GSocketService *socket_service;
	GObject *scgi_connection_manager;
	GObject *scgi_task_dispatcher;
};

G_DEFINE_TYPE(HevSCGIServer, hev_scgi_server, G_TYPE_OBJECT);

static void hev_scgi_server_dispose(GObject * obj)
{
	HevSCGIServer * self = HEV_SCGI_SERVER(obj);
	HevSCGIServerPrivate * priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->scgi_task_dispatcher)
	{
		g_object_unref(G_OBJECT(priv->scgi_task_dispatcher));
		priv->scgi_task_dispatcher = NULL;
	}

	if(priv->scgi_connection_manager)
	{
		g_object_unref(G_OBJECT(priv->scgi_connection_manager));
		priv->scgi_connection_manager = NULL;
	}

	if(priv->socket_service)
	{
		g_object_unref(G_OBJECT(priv->socket_service));
		priv->socket_service = NULL;
	}

	if(priv->scgi_config)
	{
		g_object_unref(G_OBJECT(priv->scgi_config));
		priv->scgi_config = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_server_parent_class)->dispose(obj);
}

static void hev_scgi_server_finalize(GObject * obj)
{
	HevSCGIServer * self = HEV_SCGI_SERVER(obj);
	HevSCGIServerPrivate * priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_server_parent_class)->finalize(obj);
}

static GObject * hev_scgi_server_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_server_parent_class)->constructor(type, n, param);
}

static void hev_scgi_server_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_server_parent_class)->constructed(obj);
}

static void hev_scgi_server_class_init(HevSCGIServerClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_server_constructor;
	obj_class->constructed = hev_scgi_server_constructed;
	obj_class->dispose = hev_scgi_server_dispose;
	obj_class->finalize = hev_scgi_server_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGIServerPrivate));
}

static void hev_scgi_server_init(HevSCGIServer * self)
{
	HevSCGIServerPrivate * priv = HEV_SCGI_SERVER_GET_PRIVATE(self);
	GSocketAddress *socket_address = NULL;
	GError *error = NULL;
	GObject *handler_default = NULL;
	const gchar *module_dir_path = NULL;
	const gchar *module_file_name = NULL;
	GDir *module_dir = NULL;
	GSList *module_slist = NULL, *module_sl = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->scgi_config = hev_scgi_config_new();
	if(!priv->scgi_config)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	socket_address =
		hev_scgi_config_get_address(HEV_SCGI_CONFIG(priv->scgi_config));
	if(!socket_address)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->socket_service = g_socket_service_new();
	if(!priv->socket_service)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_signal_connect(G_OBJECT(priv->socket_service), "incoming",
				G_CALLBACK(socket_service_incoming_handler), self);

	if(!g_socket_listener_add_address(G_SOCKET_LISTENER(priv->socket_service),
					socket_address, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT,
					NULL, NULL, &error))
	{
		g_critical("%s:%d[%s]=>(%s)", __FILE__, __LINE__, __FUNCTION__,
					error->message);
		g_error_free(error);
	}

	priv->scgi_connection_manager = hev_scgi_connection_manager_new();
	if(!priv->scgi_connection_manager)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_signal_connect(G_OBJECT(priv->scgi_connection_manager), "new_task",
				G_CALLBACK(scgi_connection_manager_new_task_handler), self);

	priv->scgi_task_dispatcher = hev_scgi_task_dispatcher_new();
	if(!priv->scgi_task_dispatcher)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	/* Add handler-module */
	module_dir_path = hev_scgi_config_get_module_dir_path(
				HEV_SCGI_CONFIG(priv->scgi_config));
	if(!module_dir_path)
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		return;
	}
	module_dir = g_dir_open(module_dir_path, 0, &error);
	if(!module_dir)
	{
		g_critical("%s:%d[%s]=>(%s)", __FILE__, __LINE__, __FUNCTION__,
					error->message);
		g_error_free(error);
		return;
	}

	while(module_file_name = g_dir_read_name(module_dir))
	{
		if(g_str_has_prefix(module_file_name, "lib")
					&& g_str_has_suffix(module_file_name, "so"))
		{
			gchar *module_file_path = NULL;

			module_file_path = g_module_build_path(module_dir_path,
						module_file_name);
			module_slist = g_slist_append(module_slist, module_file_path);
		}
	}

	module_slist = g_slist_sort(module_slist, (GCompareFunc)g_strcmp0);

	for(module_sl=module_slist; module_sl; module_sl=g_slist_next(module_sl))
	{
		GObject *handler_module = NULL;

		handler_module = hev_scgi_handler_module_new(module_sl->data);
		hev_scgi_task_dispatcher_add_handler(HEV_SCGI_TASK_DISPATCHER(priv->scgi_task_dispatcher),
					handler_module);
	}

	g_dir_close(module_dir);
	g_slist_free_full(module_slist, g_free);

	/* Add handler-default */
	handler_default = hev_scgi_handler_default_new();
	hev_scgi_task_dispatcher_add_handler(HEV_SCGI_TASK_DISPATCHER(priv->scgi_task_dispatcher),
				handler_default);
}

GObject * hev_scgi_server_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_SERVER, NULL);
}

void hev_scgi_server_start(HevSCGIServer *self)
{
	HevSCGIServerPrivate * priv = NULL;

	g_return_if_fail(HEV_IS_SCGI_SERVER(self));
	priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_socket_service_start(priv->socket_service);
}

void hev_scgi_server_stop(HevSCGIServer *self)
{
	HevSCGIServerPrivate * priv = NULL;

	g_return_if_fail(HEV_IS_SCGI_SERVER(self));
	priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_socket_service_stop(priv->socket_service);
}

static gboolean socket_service_incoming_handler(GSocketService *service,
			GSocketConnection *connection, GObject *source_object,
			gpointer user_data)
{
	HevSCGIServer *self = HEV_SCGI_SERVER(user_data);
	HevSCGIServerPrivate *priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	hev_scgi_connection_manager_take_over(HEV_SCGI_CONNECTION_MANAGER(
					priv->scgi_connection_manager), connection);

	return TRUE;
}

static void scgi_connection_manager_new_task_handler(HevSCGIConnectionManager *connection_manager,
			GObject *scgi_task, gpointer user_data)
{
	HevSCGIServer *self = HEV_SCGI_SERVER(user_data);
	HevSCGIServerPrivate *priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	hev_scgi_task_dispatcher_push(HEV_SCGI_TASK_DISPATCHER(priv->scgi_task_dispatcher),
				scgi_task);
}

