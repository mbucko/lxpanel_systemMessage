#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib/gi18n.h>

#include <lxpanel/plugin.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <sys/types.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <errno.h>

// internal to the plugin source, not used by the 'priv' variable
static int   iInstanceCount = 0;
static guint timerEventSource;
static char  systemMessage[256] = {'\0'};

static gboolean systemMessage_timerCallback(gpointer user_data);
static int      getSystemMessage(char msgBuffer[256]);

/* the plugin's id – an instance of this struct 
   is what will be assigned to 'priv' */
typedef struct 
{
	gint iMyId;
}SystemMessagePlugin;

static int getSystemMessage(char msgBuffer[256])
{
	FILE *fp;
	int success;
	
	// open the command for reading.
	fp = popen("dmesg | tail -n 1 2>&1", "r");
	
	if (fp == NULL)
	{
		return 0;
	}
	
	// read the output, a line at a time - output it to msgBuffer.
	success = (fgets(msgBuffer, 256, fp) != NULL);

	// cleanup
	pclose(fp);
	
	return success;
}
static gboolean systemMessage_timerCallback(gpointer user_data)
{
	GtkLabel *pLabel = (GtkLabel *)user_data;
	
	getSystemMessage(systemMessage);
	gtk_label_set_text(pLabel, systemMessage);
	
	return TRUE;
}

static int systemMessage_constructor(Plugin * p, char ** fp)
{
	/* fp is a pointer to the configuration file stream
     since we don't use it, we'll make sure it doesn't
     give us an error at compilation time */
	(void)fp;

	// allocate our private structure instance
	SystemMessagePlugin *pTest = g_new0(SystemMessagePlugin, 1);

	// put it where it belongs
	p->priv = pTest;

	// update the instance count
	pTest->iMyId = ++iInstanceCount;

	// get the latest system message
	getSystemMessage(systemMessage);
	
	// create a label containing the message
	GtkWidget *pLabel = gtk_label_new(systemMessage);

	// gray color
	GdkColor color_fg;
	gdk_color_parse("#CCCCCC", &color_fg);
	
	// set the font color of the label
	gtk_widget_modify_fg(pLabel, GTK_STATE_NORMAL, &color_fg);

	// align left
	gtk_misc_set_alignment((GtkMisc *)pLabel, 0.0, 0.0);
 
	// need to create a widget to show
	p->pwid = gtk_event_box_new();

	// set border width
	gtk_container_set_border_width(GTK_CONTAINER(p->pwid), 1);

	// add the label to the container
	gtk_container_add(GTK_CONTAINER(p->pwid), GTK_WIDGET(pLabel));

	// set the size we want
	gtk_widget_set_size_request(p->pwid, 158, 25);

	// our widget doesn't have a window...
	gtk_widget_set_has_window(p->pwid, FALSE);

	// show our widget
	gtk_widget_show_all(p->pwid);

	// crete timer callback to update the labels text - 1s interval
	timerEventSource = g_timeout_add(1000, systemMessage_timerCallback, (gpointer)pLabel);

	return 1;
}
static void systemMessage_destructor(Plugin * p)
{
	// stop the timer callback
	g_source_remove(timerEventSource);
	
	// decrement our local instance count
	--iInstanceCount;

	// find our private structure instance
	SystemMessagePlugin *pTest = (SystemMessagePlugin *)p->priv;

	// free it  -- no need to worry about freeing 'pwid', the panel does it
	g_free(pTest);
}
static void systemMessage_configure(Plugin * p, GtkWindow * parent)
{
	// doing nothing here, so make sure neither of the parameters
	// emits a warning at compilation
	(void)p;
	(void)parent;
}

static void systemMessage_save_configuration(Plugin * p, FILE * fp)
{
	// doing nothing here, so make sure neither of the parameters
	// emits a warning at compilation
	(void)p;
	(void)fp;
}

/* Plugin descriptor. */
PluginClass systemMessage_plugin_class =
{
	// this is a #define taking care of the size/version variables
	PLUGINCLASS_VERSIONING,

	// type of this plugin
	type : "systemMessage",
	name : N_("System Messages"),
	version: "1.0",
	description : N_("Show System Messages"),

	// we can have many running at the same time
	one_per_system : FALSE,

	// can't expand this plugin
	expand_available : TRUE,

	// assigning our functions to provided pointers.
	constructor : systemMessage_constructor,
	destructor  : systemMessage_destructor,
	config : systemMessage_configure,
	save : systemMessage_save_configuration
};
