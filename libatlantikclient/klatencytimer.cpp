/*
 *  Copyright (C) 2003 Thiago Macieira <thiagom@mail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qhostaddress.h>
#include <qsocketnotifier.h>

#include <ksockaddr.h>

#ifdef USE_QT_ADDON
# include <qsocketaddress.h>
#endif

#include "klatencytimer.h"

/*
 * temporary class exposing the needed functionality
 */
#ifdef USE_QT_ADDON
class SocketAddress : public QInetSocketAddress
{
public:
  void setHost(const QString& host)
  { QInetSocketAddress::setHost(QIpAddress(host)); }

  void setHost(const in_addr& addr)
  { QInetSocketAddress::setHost(QIpAddress(&addr, 4)); }

  void setHost(const in6_addr& addr)
  { QInetSocketAddress::setHost(QIpAddress(&addr, 6)); }

  //using QInetAddress::setHost(const QIpAddress&);
  void setHost(const QIpAddress& addr)
  { QInetSocketAddress::setHost(addr); }

  unsigned size() const
  { return length(); }
};
#else
class SocketAddress : public KInetSocketAddress
{
};
#endif

class KLatencyTimerPrivate
{
public:
  SocketAddress target;
  QTime time;
  QSocketNotifier* notifier;
  int sock;
  int elapsed;

  KLatencyTimerPrivate()
    : notifier(0L)
  {
  }
};

KLatencyTimer::KLatencyTimer(int port, QObject* parent, const char* name)
  : QObject(parent, name), m_port(port), m_running(false), m_error(false),
    d(new KLatencyTimerPrivate)
{
}

KLatencyTimer::~KLatencyTimer()
{
  cancel();
  delete d;
}

void KLatencyTimer::setPort(int port)
{
  if (m_running)
    return;

  if (port < 0 || port > 0xffff)
    m_port = -1;
  else
    m_port = port;
}

void KLatencyTimer::setHost(const QString& node)
{
  d->target.setHost(node);
}

void KLatencyTimer::setHost(const QHostAddress& node)
{
  if (!node.isIp4Addr())
    return;
  in_addr addr;
  addr.s_addr = node.ip4Addr();
  d->target.setHost(addr);
}

#ifdef USE_QT_ADDON
void KLatencyTimer::setHost(const QIpAddress& node)
{
  d->target.setHost(node);
}

void KLatencyTimer::setHost(const QSocketAddress& node)
{
  if (node.inet().ipVersion() != 0)
    d->target.setHost(node.inet().ipAddress());
}
#endif

void KLatencyTimer::setHost(const KSocketAddress* node)
{
  const KInetSocketAddress *nodeinet = dynamic_cast<const KInetSocketAddress*>(node);
  if (nodeinet == 0L)
    return;
  if (nodeinet->size() == sizeof(sockaddr_in))
    d->target.setHost(nodeinet->hostV4());
  else
    d->target.setHost(nodeinet->hostV6());
}

void KLatencyTimer::start()
{
  if (m_running)
    return;			// already running

  d->sock = ::socket(d->target.family(), SOCK_DGRAM, IPPROTO_UDP);
  if (d->sock == -1)
    {
      m_error = true;
      return;			// error
    }

  // set non-blocking mode
  int fdflags = fcntl(d->sock, F_GETFL, 0);
  if (fdflags == -1)
    {
      m_error = true;
      return;			// error
    }
  fdflags |= O_NONBLOCK;

  if (fcntl(d->sock, F_SETFL, fdflags) == -1)
    {
      m_error = true;
      return;			// error
    }

  d->notifier = new QSocketNotifier(d->sock, QSocketNotifier::Read);
  d->notifier->setEnabled(true);
  connect(d->notifier, SIGNAL(activated(int)), this, SLOT(activity()));

  if (m_port != -1)
    d->target.setPort(m_port);
  else
    d->target.setPort(rand());

  ::connect(d->sock, d->target.address(), d->target.size());

  // now send a dummy packet to the destination
  static const char dummydata[] = "PING";
  m_running = true;
  m_error = false;
  d->time.start();
  ::write(d->sock, dummydata, sizeof dummydata - 1);
}

int KLatencyTimer::elapsed() const
{
  if (m_running)
    return d->time.elapsed();
  else
    return d->elapsed;
}

void KLatencyTimer::cancel()
{
  if (!m_running)
    return;

  delete d->notifier;
  ::close(d->sock);
  m_running = false;
  m_error = false;
}

void KLatencyTimer::activity()
{
  if (!m_running)
    return;			// error!

  // retrieve the socket status
  int errcode;
  socklen_t len = sizeof(errcode);
  if (getsockopt(d->sock, SOL_SOCKET, SO_ERROR, (char*)&errcode, &len) == -1)
    {
      m_error = true;
      return; 			// error!
    }

  if (errcode != ECONNREFUSED)
    m_error = true;
  else
    m_error = false;

  d->elapsed = d->time.elapsed();
  delete d->notifier;
  close(d->sock);
  m_running = false;

  emit answer(d->elapsed);
}

#include "klatencytimer.moc"
