/* gtd-task.c
 *
 * Copyright (C) 2015 Georges Basile Stavracas Neto <georges.stavracas@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtd-task.h"
#include "gtd-task-list.h"

#include <glib/gi18n.h>

typedef struct
{
  gboolean         complete;
  gchar           *title;
  gchar           *description;
  gint             position;
  GDateTime       *dt;
  GtdTaskList     *list;
} GtdTaskPrivate;

struct _GtdTask
{
  GtdObject parent;

  /*<private>*/
  GtdTaskPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtdTask, gtd_task, GTD_TYPE_OBJECT)

enum
{
  PROP_0,
  PROP_COMPLETE,
  PROP_DESCRIPTION,
  PROP_DUE_DATE,
  PROP_LIST,
  PROP_POSITION,
  PROP_TITLE,
  LAST_PROP
};

static void
gtd_task_finalize (GObject *object)
{
  GtdTask *self = (GtdTask*) object;

  if (self->priv->title)
    g_free (self->priv->title);

  if (self->priv->description)
    g_free (self->priv->description);

  if (self->priv->dt)
    g_date_time_unref (self->priv->dt);

  G_OBJECT_CLASS (gtd_task_parent_class)->finalize (object);
}

static void
gtd_task_get_property (GObject    *object,
                       guint       prop_id,
                       GValue     *value,
                       GParamSpec *pspec)
{
  GtdTask *self = GTD_TASK (object);

  switch (prop_id)
    {
    case PROP_COMPLETE:
      g_value_set_boolean (value, self->priv->complete);
      break;

    case PROP_DESCRIPTION:
      g_value_set_string (value, self->priv->description);
      break;

    case PROP_DUE_DATE:
      g_value_set_boxed (value, self->priv->dt);
      break;

    case PROP_LIST:
      g_value_set_boxed (value, self->priv->list);
      break;

    case PROP_POSITION:
      g_value_set_int (value, self->priv->position);
      break;

    case PROP_TITLE:
      g_value_set_string (value, self->priv->title);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_task_set_property (GObject      *object,
                       guint         prop_id,
                       const GValue *value,
                       GParamSpec   *pspec)
{
  GtdTask *self = GTD_TASK (object);

  switch (prop_id)
    {
    case PROP_COMPLETE:
      gtd_task_set_complete (self, g_value_get_boolean (value));
      break;

    case PROP_DESCRIPTION:
      gtd_task_set_description (self, g_value_get_string (value));
      break;

    case PROP_DUE_DATE:
      gtd_task_set_due_date (self, g_value_get_boxed (value));
      break;

    case PROP_LIST:
      gtd_task_set_list (self, g_value_get_boxed (value));
      break;

    case PROP_POSITION:
      gtd_task_set_position (self, g_value_get_int (value));
      break;

    case PROP_TITLE:
      gtd_task_set_title (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gtd_task_class_init (GtdTaskClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gtd_task_finalize;
  object_class->get_property = gtd_task_get_property;
  object_class->set_property = gtd_task_set_property;

  /**
   * GtdTask::complete:
   *
   * @TRUE if the task is marked as complete or @FALSE otherwise. Usually
   * represented by a checkbox at user interfaces.
   */
  g_object_class_install_property (
        object_class,
        PROP_COMPLETE,
        g_param_spec_boolean ("complete",
                              _("Whether the task is completed or not"),
                              _("Whether the task is marked as completed by the user"),
                              FALSE,
                              G_PARAM_READWRITE));

  /**
   * GtdTask::description:
   *
   * Description of the task.
   */
  g_object_class_install_property (
        object_class,
        PROP_DESCRIPTION,
        g_param_spec_string ("description",
                             _("Description of the task"),
                             _("Optional string describing the task"),
                             NULL,
                             G_PARAM_READWRITE));

  /**
   * GtdTask::due-date:
   *
   * The @GDateTime that represents the time in which the task should
   * be completed before.
   */
  g_object_class_install_property (
        object_class,
        PROP_DUE_DATE,
        g_param_spec_boxed ("due-date",
                            _("End date of the task"),
                            _("The day the task is supposed to be completed"),
                            G_TYPE_DATE_TIME,
                            G_PARAM_READWRITE));

  /**
   * GtdTask::list:
   *
   * The @GtdTaskList that contains this task.
   */
  g_object_class_install_property (
        object_class,
        PROP_LIST,
        g_param_spec_boxed ("list",
                            _("List of the task"),
                            _("The list that owns this task"),
                            GTD_TYPE_TASK_LIST,
                            G_PARAM_READWRITE));

  /**
   * GtdTask::position:
   *
   * Position of the task at the list, -1 if not set.
   */
  g_object_class_install_property (
        object_class,
        PROP_POSITION,
        g_param_spec_int ("position",
                          _("Position of the task"),
                          _("The position of the task regarding the list. -1 means no position set, and tasks will be sorted alfabetically."),
                          -1,
                          G_MAXINT,
                          -1,
                          G_PARAM_READWRITE));

  /**
   * GtdTask::title:
   *
   * The title of the task, usually the task name.
   */
  g_object_class_install_property (
        object_class,
        PROP_TITLE,
        g_param_spec_string ("title",
                             _("Title of the task"),
                             _("The title of the task"),
                             NULL,
                             G_PARAM_READWRITE));
}

static void
gtd_task_init (GtdTask *self)
{
  self->priv = gtd_task_get_instance_private (self);
}

GtdTask *
gtd_task_new (const gchar* uid)
{
  return g_object_new (GTD_TYPE_TASK, "uid", uid, NULL);
}

/**
 * gtd_task_get_complete:
 * @task: a #GtdTask
 *
 * Retrieves whether the task is complete or not.
 *
 * Returns: %TRUE if the task is complete, %FALSE otherwise
 */
gboolean
gtd_task_get_complete (GtdTask *task)
{
  g_return_if_fail (GTD_IS_TASK (task));

  return task->priv->complete;
}

/**
 * gtd_task_set_complete:
 * @task: a #GtdTask
 * @complete: the new value
 *
 * Updates the complete state of @task.
 *
 * Returns:
 */
void
gtd_task_set_complete (GtdTask  *task,
                       gboolean  complete)
{
  g_assert (GTD_IS_TASK (task));

  if (task->priv->complete != complete)
    {
      task->priv->complete = complete;
      g_object_notify (G_OBJECT (task), "complete");
    }
}

/**
 * gtd_task_get_description:
 * @task: a #GtdTask
 *
 * Retrieves the description of the task, or %NULL.
 *
 * Returns: (transfer none): the description of @task, or %NULL
 */
const gchar*
gtd_task_get_description (GtdTask *task)
{
  g_return_val_if_fail (GTD_IS_TASK (task), NULL);

  return task->priv->description;
}

/**
 * gtd_task_set_description:
 * @task: a #GtdTask
 * @description: the new description, or %NULL
 *
 * Updates the description of @task. The string is not stripped off of
 * spaces to preserve user data.
 *
 * Returns:
 */
void
gtd_task_set_description (GtdTask     *task,
                          const gchar *description)
{
  g_assert (GTD_IS_TASK (task));
  g_assert (g_utf8_validate (description, -1, NULL));

  if (g_strcmp0 (task->priv->description, description) != 0)
    {
      if (task->priv->description)
        g_free (task->priv->description);

      task->priv->description = g_strdup (description);

      g_object_notify (G_OBJECT (task), "description");
    }
}

/**
 * gtd_task_get_due_date:
 * @task: a #GtdTask
 *
 * Returns the #GDateTime that represents the task's due date.
 * The value is referenced for thread safety. Returns %NULL if
 * no date is set.
 *
 * Returns: (transfer full): the internal #GDateTime referenced
 * for thread safety, or %NULL. Unreference it after use.
 */
GDateTime*
gtd_task_get_due_date (GtdTask *task)
{
  g_return_val_if_fail (GTD_IS_TASK (task), NULL);

  if (task->priv->dt == NULL)
    return NULL;

  return g_date_time_ref (task->priv->dt);
}

/**
 * gtd_task_set_due_date:
 * @task: a #GtdTask
 * @dt: a #GDateTime
 *
 * Updates the internal @GtdTask::due-date property.
 *
 * Returns:
 */
void
gtd_task_set_due_date (GtdTask   *task,
                       GDateTime *dt)
{
  g_assert (GTD_IS_TASK (task));

  if (dt != task->priv->dt)
    {
      if (dt &&
          !(task->priv->dt && g_date_time_compare (task->priv->dt, dt)))
        {
          /* Get rid of the old GDateTime for this task */
          if (task->priv->dt)
            g_date_time_unref (task->priv->dt);

          g_date_time_ref (dt);

          /* Copy the given dt */
          task->priv->dt = g_date_time_new_local (
                  g_date_time_get_year (dt),
                  g_date_time_get_month (dt),
                  g_date_time_get_day_of_month (dt),
                  g_date_time_get_hour (dt),
                  g_date_time_get_minute (dt),
                  g_date_time_get_seconds (dt));

          g_date_time_unref (dt);
        }
      else if (!dt)
        {
          if (task->priv->dt)
            g_date_time_unref (task->priv->dt);

          task->priv->dt = NULL;
        }

      g_object_notify (G_OBJECT (task), "due-date");
    }
}

/**
 * gtd_task_get_list:
 *
 * Returns a weak reference to the #GtdTaskList that
 * owns the given @task.
 *
 * Returns: (transfer none): a weak reference to the
 * #GtdTaskList that owns @task. Do not free after
 * usage.
 */
GtdTaskList*
gtd_task_get_list (GtdTask *task)
{
  g_return_val_if_fail (GTD_IS_TASK (task), NULL);

  return task->priv->list;
}

/**
 * gtd_task_set_list:
 *
 * Sets the parent #GtdTaskList of @task.
 *
 * Returns:
 */
void
gtd_task_set_list (GtdTask     *task,
                   GtdTaskList *list)
{
  g_assert (GTD_IS_TASK (task));
  g_assert (GTD_IS_TASK_LIST (list));

  if (task->priv->list != list)
    {
      task->priv->list = list;
      g_object_notify (G_OBJECT (task), "list");
    }
}

/**
 * gtd_task_get_position:
 * @task: a #GtdTask
 *
 * Returns the position of @task inside the parent #GtdTaskList,
 * or -1 if not set.
 *
 * Returns: the position of the task, or -1
 */
gint
gtd_task_get_position (GtdTask *task)
{
  g_assert (GTD_IS_TASK (task));

  return task->priv->position;
}

/**
 * gtd_task_set_position:
 * @task: a #GtdTask
 * @position: the position of @task, or -1
 *
 * Sets the @task position inside the parent #GtdTaskList. It
 * is up to the interface to handle two or more #GtdTask with
 * the same position value.
 *
 * Returns:
 */
void
gtd_task_set_position (GtdTask *task,
                       gint     position)
{
  g_assert (GTD_IS_TASK (task));
  g_assert (position >= -1);

  if (position != task->priv->position)
    {
      task->priv->position = position;
      g_object_notify (G_OBJECT (task), "position");
    }
}

/**
 * gtd_task_get_title:
 * @task: a #GtdTask
 *
 * Retrieves the title of the task, or %NULL.
 *
 * Returns: (transfer none): the title of @task, or %NULL
 */
const gchar*
gtd_task_get_title (GtdTask *task)
{
  g_return_val_if_fail (GTD_IS_TASK (task), NULL);

  return task->priv->title;
}

/**
 * gtd_task_set_title:
 * @task: a #GtdTask
 * @description: the new title, or %NULL
 *
 * Updates the title of @task. The string is stripped off of
 * leading spaces.
 *
 * Returns:
 */
void
gtd_task_set_title (GtdTask     *task,
                    const gchar *title)
{
  g_assert (GTD_IS_TASK (task));
  g_assert (g_utf8_validate (title, -1, NULL));

  if (g_strcmp0 (task->priv->title, title) != 0)
    {
      if (task->priv->title)
        g_free (task->priv->title);

      task->priv->title = g_strdup (title);

      g_object_notify (G_OBJECT (task), "title");
    }
}