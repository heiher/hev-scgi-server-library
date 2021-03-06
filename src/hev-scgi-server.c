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
#include "hev-scgi-task.h"
#include "hev-scgi-request.h"
#include "hev-scgi-response.h"
#include "hev-scgi-connection-manager.h"
#include "hev-scgi-task-dispatcher.h"
#include "hev-scgi-handler-module.h"
#include "hev-scgi-handler-default.h"

enum
{
	PROP_0,
	PROP_CONF_DIR,
	N_PROPERTIES
};

static GParamSpec *hev_scgi_server_properties[N_PROPERTIES] = { NULL };

static gboolean scgi_connection_manager_incoming_handler(GSocketService *service,
			GSocketConnection *connection, GObject *source_object, gpointer user_data);

#define HEV_SCGI_SERVER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_SERVER, HevSCGIServerPrivate))

typedef struct _HevSCGIServerPrivate HevSCGIServerPrivate;

struct _HevSCGIServerPrivate
{
	gchar *conf_dir;

	GObject *scgi_config;
	GObject *scgi_connection_manager;
	GObject *scgi_task_dispatcher;
	GSList *module_slist;
};

G_DEFINE_TYPE(HevSCGIServer, hev_scgi_server, G_TYPE_OBJECT);

static void hev_scgi_server_dispose(GObject * obj)
{
	HevSCGIServer * self = HEV_SCGI_SERVER_CAST(obj);
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

	if(priv->scgi_config)
	{
		g_object_unref(G_OBJECT(priv->scgi_config));
		priv->scgi_config = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_server_parent_class)->dispose(obj);
}

static void hev_scgi_server_finalize(GObject * obj)
{
	HevSCGIServer * self = HEV_SCGI_SERVER_CAST(obj);
	HevSCGIServerPrivate * priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->module_slist)
	{
		g_slist_free_full(priv->module_slist, (GDestroyNotify)g_type_module_unuse);
		priv->module_slist = NULL;
	}

	if(priv->conf_dir)
	{
		g_free(priv->conf_dir);
		priv->conf_dir = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_server_parent_class)->finalize(obj);
}

static GObject * hev_scgi_server_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_server_parent_class)->constructor(type, n, param);
}

static void hev_scgi_server_constructed(GObject * obj)
{
	HevSCGIServer * self = HEV_SCGI_SERVER_CAST(obj);
	HevSCGIServerPrivate * priv = HEV_SCGI_SERVER_GET_PRIVATE(self);
	GSocketAddress *socket_address = NULL;
	GError *error = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_server_parent_class)->constructed(obj);

	priv->scgi_config = hev_scgi_config_new(priv->conf_dir);
	if(!priv->scgi_config)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	socket_address =
		hev_scgi_config_get_address(HEV_SCGI_CONFIG(priv->scgi_config));
	if(!socket_address)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->scgi_connection_manager = hev_scgi_connection_manager_new();
	if(!priv->scgi_connection_manager)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(!g_socket_listener_add_address(G_SOCKET_LISTENER(priv->scgi_connection_manager),
					socket_address, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT,
					NULL, NULL, &error))
	{
		g_critical("%s:%d[%s]=>(%s)", __FILE__, __LINE__, __FUNCTION__,
					error->message);
		g_error_free(error);
	}

	g_signal_connect(G_OBJECT(priv->scgi_connection_manager), "incoming",
				G_CALLBACK(scgi_connection_manager_incoming_handler), self);

	priv->scgi_task_dispatcher = hev_scgi_task_dispatcher_new();
	if(!priv->scgi_task_dispatcher)
	  g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

static void hev_scgi_server_set_property(GObject *obj,
			guint prop_id, const GValue *value, GParamSpec *pspec)
{
	HevSCGIServer * self = HEV_SCGI_SERVER_CAST(obj);
	HevSCGIServerPrivate * priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	switch(prop_id)
	{
	case PROP_CONF_DIR:
		priv->conf_dir = g_value_dup_string(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
		break;
	}
}

static void hev_scgi_server_get_property(GObject *obj,
			guint prop_id, GValue *value, GParamSpec *pspec)
{
	HevSCGIServer * self = HEV_SCGI_SERVER_CAST(obj);
	HevSCGIServerPrivate * priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	switch(prop_id)
	{
	case PROP_CONF_DIR:
		g_value_set_string(value, priv->conf_dir);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop_id, pspec);
		break;
	}
}

static void hev_scgi_server_class_init(HevSCGIServerClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_server_constructor;
	obj_class->constructed = hev_scgi_server_constructed;
	obj_class->set_property = hev_scgi_server_set_property;
	obj_class->get_property = hev_scgi_server_get_property;
	obj_class->dispose = hev_scgi_server_dispose;
	obj_class->finalize = hev_scgi_server_finalize;

	hev_scgi_server_properties[PROP_CONF_DIR] =
		g_param_spec_string ("conf-dir",
					"Configure Directory",
					"The directory path to use when loading config file.",
					NULL,
					G_PARAM_READWRITE |	G_PARAM_CONSTRUCT_ONLY);
	g_object_class_install_properties(obj_class, N_PROPERTIES,
				hev_scgi_server_properties);

	g_type_class_add_private(klass, sizeof(HevSCGIServerPrivate));
}

static void hev_scgi_server_init(HevSCGIServer * self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

/**
 * hev_scgi_server_new
 * @conf_dir: Config directory path
 *
 * Creates a server.
 *
 * Returns: (transfer full)(type HevSCGIServer): A #HevSCGIServer.
 *
 * Since: 0.0.1
 */
GObject * hev_scgi_server_new(const gchar *conf_dir)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_SERVER, "conf-dir", conf_dir, NULL);
}

/**
 * hev_scgi_server_start
 * @self: A #HevSCGIServer
 *
 * Start the server.
 *
 * Since: 0.0.1
 */
void hev_scgi_server_start(HevSCGIServer *self)
{
	HevSCGIServerPrivate * priv = NULL;

	g_return_if_fail(HEV_IS_SCGI_SERVER(self));
	priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_socket_service_start(G_SOCKET_SERVICE(priv->scgi_connection_manager));
}

/**
 * hev_scgi_server_stop
 * @self: A #HevSCGIServer
 *
 * Stop the server.
 *
 * Since: 0.0.1
 */
void hev_scgi_server_stop(HevSCGIServer *self)
{
	HevSCGIServerPrivate * priv = NULL;

	g_return_if_fail(HEV_IS_SCGI_SERVER(self));
	priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	g_socket_service_stop(G_SOCKET_SERVICE(priv->scgi_connection_manager));
}

/**
 * hev_scgi_server_load_extern_handlers
 * @self: A #HevSCGIServer
 *
 * Load external handlers into server from modules confifg file.
 *
 * Since: 1.0.1
 */
void hev_scgi_server_load_extern_handlers(HevSCGIServer *self)
{
	HevSCGIServerPrivate *priv = HEV_SCGI_SERVER_GET_PRIVATE(self);
	gchar *module_dir_path = NULL;
	GSList *module_slist = NULL, *module_sl = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	module_dir_path = hev_scgi_config_get_module_dir_path(
				HEV_SCGI_CONFIG(priv->scgi_config));
	if(!module_dir_path)
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	/* Add handler-module */
	module_slist = hev_scgi_config_get_modules(HEV_SCGI_CONFIG(priv->scgi_config));
	for(module_sl=module_slist; module_sl; module_sl=g_slist_next(module_sl))
	{
		GObject *handler_module = NULL;
		GType handler_type = G_TYPE_INVALID;
		GObject *handler = NULL;
		GKeyFile *config = NULL;
		gchar *file_name = NULL;
		gchar *type_name = NULL;
		gchar *file_path = NULL;

		config = hev_scgi_config_get_module_config(HEV_SCGI_CONFIG(priv->scgi_config),
					(const gchar *)module_sl->data);

		file_name = g_key_file_get_string(config, "Module", "FileName", NULL);
		type_name = g_key_file_get_string(config, "Module", "TypeName", NULL);
		if(file_name)
		{
			file_path = g_build_path(G_DIR_SEPARATOR_S, module_dir_path, file_name, NULL);
			handler_module = hev_scgi_handler_module_new(file_path);
			g_type_module_use(G_TYPE_MODULE(handler_module));
			priv->module_slist = g_slist_append(priv->module_slist, handler_module);
			handler_type = hev_scgi_handler_module_get_handler_type(HEV_SCGI_HANDLER_MODULE(handler_module));
			g_free(file_name);
			g_free(file_path);
		}
		else if (type_name)
		{
			handler_type = g_type_from_name (type_name);
			g_free(type_name);
		}

		if(G_TYPE_INVALID != handler_type)
		{
			handler = g_object_new(handler_type, "config", config, NULL);
			hev_scgi_task_dispatcher_add_handler(HEV_SCGI_TASK_DISPATCHER(priv->scgi_task_dispatcher), handler);
		}
	}

	g_free(module_dir_path);
	g_slist_free_full(module_slist, g_free);
}

/**
 * hev_scgi_server_load_default_handler
 * @self: A #HevSCGIServer
 *
 * Load default handler into server.
 *
 * Since: 1.0.1
 */
void hev_scgi_server_load_default_handler(HevSCGIServer *self)
{
	HevSCGIServerPrivate *priv = HEV_SCGI_SERVER_GET_PRIVATE(self);
	GObject *handler_default = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	/* Add handler-default */
	handler_default = hev_scgi_handler_default_new();
	hev_scgi_task_dispatcher_add_handler(HEV_SCGI_TASK_DISPATCHER(priv->scgi_task_dispatcher),
				handler_default);
}

/**
 * hev_scgi_server_add_handler
 * @self: A #HevSCGIServer
 * @handler: (transfer full) A #HevSCGIHandler
 *
 * Add handler into server.
 *
 * Since: 1.0.1
 */
void hev_scgi_server_add_handler(HevSCGIServer *self, GObject *handler)
{
	HevSCGIServerPrivate *priv = HEV_SCGI_SERVER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	hev_scgi_task_dispatcher_add_handler(HEV_SCGI_TASK_DISPATCHER(priv->scgi_task_dispatcher),
				handler);
}

static gboolean scgi_connection_manager_incoming_handler(GSocketService *service,
			GSocketConnection *connection, GObject *source_object, gpointer user_data)
{
	HevSCGIServer *self = HEV_SCGI_SERVER_CAST(user_data);
	HevSCGIServerPrivate *priv = HEV_SCGI_SERVER_GET_PRIVATE(self);
	GObject *scgi_task = NULL;
	GObject *scgi_request = NULL;
	GObject *scgi_response = NULL;
	GInputStream *input_stream = NULL;
	GOutputStream *output_stream = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_task = _hev_scgi_task_new();
	if(!scgi_task)
	{
		g_critical("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
		return FALSE;
	}

	_hev_scgi_task_set_socket_connection(HEV_SCGI_TASK(scgi_task),
				G_OBJECT(connection));

	scgi_request = hev_scgi_task_get_request(HEV_SCGI_TASK(scgi_task));
	scgi_response = hev_scgi_task_get_response(HEV_SCGI_TASK(scgi_task));

	input_stream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
	output_stream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

	_hev_scgi_request_set_input_stream(HEV_SCGI_REQUEST(scgi_request),
				input_stream);
	_hev_scgi_response_set_output_stream(HEV_SCGI_RESPONSE(scgi_response),
				output_stream);

	hev_scgi_task_dispatcher_push(HEV_SCGI_TASK_DISPATCHER(priv->scgi_task_dispatcher),
				scgi_task);

	return TRUE;
}

