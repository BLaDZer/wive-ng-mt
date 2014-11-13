/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

void EventHistory::init()
{
}


void EventHistory::destroy()
{
}


void EventHistory::addEvents(WpaMsgList msgs)
{
    WpaMsgList::iterator it;
    for (it = msgs.begin(); it != msgs.end(); it++) {
	addEvent(*it);
    }
}


void EventHistory::addEvent(WpaMsg msg)
{
    Q3ListViewItem *item;
    item = new Q3ListViewItem(eventListView,
			     msg.getTimestamp().toString("yyyy-MM-dd hh:mm:ss.zzz"),
			     msg.getMsg());
    if (item == NULL)
	return;
    eventListView->setSelected(item, false);
}
