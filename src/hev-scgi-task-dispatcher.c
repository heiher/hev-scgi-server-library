/*
 ============================================================================
 Name        : hev-scgi-task-dispatcher.c
 Author      : Heiher <admin@heiher.info>
 Version     : 0.0.1
 Copyright   : Copyright (C) 2011 everyone.
 Description : 
 ============================================================================
 */

#include "hev-scgi-task-dispatcher.h"
#include "hev-scgi-request.h"
#include "hev-scgi-task.h"
#include "hev-scgi-handler.h"

#define HEV_SCGI_TASK_DISPATCHER_GET_PRIVATE(obj)	(G_TYPE_INSTANCE_GET_PRIVATE((obj), HEV_TYPE_SCGI_TASK_DISPATCHER, HevSCGITaskDispatcherPrivate))

typedef struct _HevClosure0 HevClosure0;
typedef struct _HevSCGITaskDispatcherPrivate HevSCGITaskDispatcherPrivate;

struct _HevClosure0
{
	GObject *task;
	HevSCGITaskDispatcher *dispatcher;
};

struct _HevSCGITaskDispatcherPrivate
{
	GSList *handler_slist;
};

G_DEFINE_TYPE(HevSCGITaskDispatcher, hev_scgi_task_dispatcher, G_TYPE_OBJECT);

static void hev_scgi_request_read_header_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data);

static void hev_scgi_task_dispatcher_dispatch(HevSCGITaskDispatcher *self,
			GObject *scgi_task);

static void hev_scgi_task_dispatcher_dispose(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_task_dispatcher_parent_class)->dispose(obj);
}

static void hev_scgi_task_dispatcher_finalize(GObject * obj)
{
	HevSCGITaskDispatcher * self = HEV_SCGI_TASK_DISPATCHER_CAST(obj);
	HevSCGITaskDispatcherPrivate * priv = HEV_SCGI_TASK_DISPATCHER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(priv->handler_slist)
	{
		g_slist_free_full(priv->handler_slist,
					g_object_unref);
		priv->handler_slist = NULL;
	}

	G_OBJECT_CLASS(hev_scgi_task_dispatcher_parent_class)->finalize(obj);
}

static GObject * hev_scgi_task_dispatcher_constructor(GType type, guint n, GObjectConstructParam * param)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return G_OBJECT_CLASS(hev_scgi_task_dispatcher_parent_class)->constructor(type, n, param);
}

static void hev_scgi_task_dispatcher_constructed(GObject * obj)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	G_OBJECT_CLASS(hev_scgi_task_dispatcher_parent_class)->constructed(obj);
}

static void hev_scgi_task_dispatcher_class_init(HevSCGITaskDispatcherClass * klass)
{
	GObjectClass * obj_class = G_OBJECT_CLASS(klass);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	obj_class->constructor = hev_scgi_task_dispatcher_constructor;
	obj_class->constructed = hev_scgi_task_dispatcher_constructed;
	obj_class->dispose = hev_scgi_task_dispatcher_dispose;
	obj_class->finalize = hev_scgi_task_dispatcher_finalize;

	g_type_class_add_private(klass, sizeof(HevSCGITaskDispatcherPrivate));
}

static void hev_scgi_task_dispatcher_init(HevSCGITaskDispatcher * self)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
}

GObject * hev_scgi_task_dispatcher_new(void)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);
	return g_object_new(HEV_TYPE_SCGI_TASK_DISPATCHER, NULL);
}

void hev_scgi_task_dispatcher_push(HevSCGITaskDispatcher *self,
			GObject *scgi_task)
{
	GObject *scgi_request = NULL;
	HevClosure0 *closure = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	closure = g_slice_new(HevClosure0);
	closure->task = scgi_task;
	closure->dispatcher = self;
	scgi_request = hev_scgi_task_get_request(HEV_SCGI_TASK(scgi_task));
	_hev_scgi_request_read_header_async(HEV_SCGI_REQUEST(scgi_request),
				NULL, hev_scgi_request_read_header_async_handler, closure);
}

static void hev_scgi_request_read_header_async_handler(GObject *source_object,
			GAsyncResult *res, gpointer user_data)
{
	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	if(_hev_scgi_request_read_header_finish(HEV_SCGI_REQUEST(source_object),
					res, NULL))
	{
		HevClosure0 *closure = user_data;
		HevSCGITaskDispatcher *self = NULL;
		
		self = HEV_SCGI_TASK_DISPATCHER_CAST(closure->dispatcher);

		hev_scgi_task_dispatcher_dispatch(self, closure->task);

		g_slice_free(HevClosure0, closure);
	}
}

static void hev_scgi_task_dispatcher_dispatch(HevSCGITaskDispatcher *self,
			GObject *scgi_task)
{
	HevSCGITaskDispatcherPrivate * priv = HEV_SCGI_TASK_DISPATCHER_GET_PRIVATE(self);
	GObject *scgi_request = NULL;
	GHashTable *header_hash_table = NULL;
	gchar *request_uri = NULL;
	GSList *sl = NULL;

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	scgi_request = hev_scgi_task_get_request(HEV_SCGI_TASK(scgi_task));
	header_hash_table = hev_scgi_request_get_header_hash_table(
				HEV_SCGI_REQUEST(scgi_request));
	request_uri = g_hash_table_lookup(header_hash_table, "REQUEST_URI");
	if(!request_uri)
	{
		g_object_unref(scgi_task);
		return;
	}

	for(sl=priv->handler_slist; sl; sl=g_slist_next(sl))
	{
		const gchar *pattern = NULL;

		pattern = hev_scgi_handler_get_pattern(HEV_SCGI_HANDLER_CAST(sl->data));
		if(g_regex_match_simple(pattern, request_uri, 0, 0))
		{
			_hev_scgi_task_set_handler(HEV_SCGI_TASK(scgi_task), G_OBJECT(sl->data));
			hev_scgi_handler_handle(HEV_SCGI_HANDLER_CAST(sl->data), scgi_task);
			break;
		}
	}

	g_object_unref(scgi_task);
}

void hev_scgi_task_dispatcher_add_handler(HevSCGITaskDispatcher *self,
			GObject *scgi_handler)
{
	HevSCGITaskDispatcherPrivate * priv = HEV_SCGI_TASK_DISPATCHER_GET_PRIVATE(self);

	g_debug("%s:%d[%s]", __FILE__, __LINE__, __FUNCTION__);

	priv->handler_slist = g_slist_append(priv->handler_slist,
				scgi_handler);
	g_message("Task Dispatcher: AddHandler[%s, %s, %s]",
				hev_scgi_handler_get_alias(HEV_SCGI_HANDLER_CAST(scgi_handler)),
				hev_scgi_handler_get_name(HEV_SCGI_HANDLER_CAST(scgi_handler)),
				hev_scgi_handler_get_version(HEV_SCGI_HANDLER_CAST(scgi_handler)));
}

