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
#ifndef KLATENCYTIMER_H
#define KLATENCYTIMER_H

#include <qobject.h>
#include <qstring.h>

/////////////
// forward declarations

// POSIX structs
struct in_addr;
struct in6_addr;

// from standard Qt
class QHostAddress;

// from libqt-addon
class QIpAddress;
class QSocketAddress;

// from KDE
class KSocketAddress;

class KLatencyTimerPrivate;
/**
 * Simple measurement of latency, similar to ping.
 *
 * This class does a simple measurement of the latency to reach
 * the remote host. It is similar in behaviour to the system's
 * ping command, but it's not a ping and it also does not execute
 * any external programs.
 *
 * The method used consists of sending a dummy UDP packet to a
 * given or random port on the target host. When we receive the
 * reply telling us that the port is unreachable, we calculate
 * the time it took.
 *
 * @warning The method will time-out if we end up hitting an open
 *          port or if it's blocked by firewall. Also note that
 *          if we hit an open port, the other side may not like
 *          the dummy packet we sent.
 *
 * @author Thiago Macieira
 * @version 0.9
 * $Id$
 */
class KLatencyTimer: public QObject
{
  Q_OBJECT

public:
  /**
   * Default constructor.
   *
   * @param port	the port to send the packet to
   *			or -1 to choose one randomly
   */
  explicit KLatencyTimer(int port = -1, QObject* parent = 0L, const char *name = 0L);

  /**
   * Destructor.
   */
  virtual ~KLatencyTimer();

  /**
   * Retrieves the stored port number
   */
  int port() const
  { return m_port; }

  /**
   * Sets the port to the given value. Use value -1 to
   * make the class select a port number.
   *
   * @param port	the port to send the packet to
   *			or -1 to choose one randomly
   */
  void setPort(int port);

  /**
   * Retrieves the host name
   */
  QString host() const;

  /**
   * Sets the target host. The @p host parameter must be
   * the string representation of a numeric IP address
   * (v4 or v6). Host names must be resolved by other
   * lookup means.
   *
   * @param node	the target host
   */
  void setHost(const QString& node);

  /**
   * @overload
   *
   * Sets the target host.
   *
   * @param node	the target host
   */
  void setHost(const in_addr& node);

  /**
   * @overload
   *
   * Sets the target host.
   *
   * @param node	the target host
   */
  void setHost(const in6_addr& node);

  /**
   * @overload
   *
   * Sets the target host.
   *
   * @param node	the target host
   */
  void setHost(const QHostAddress& node);

  /**
   * @overload
   *
   * Sets the target host.
   *
   * @param node	the target host
   */
  void setHost(const QIpAddress& node);

  /**
   * @overload
   *
   * Sets the target host from the host component
   * of the socket address.
   *
   * @param node	the target host
   */
  void setHost(const QSocketAddress& node);

  /**
   * @overload
   *
   * Sets the target host from the host component
   * of the socket address.
   *
   * @param node	the target host
   */
  void setHost(const KSocketAddress* node);

  /**
   * Starts the measurement.
   */
  void start();

  /**
   * Returns true if there has been a reply from the
   * target host.
   */
  bool finished() const
  { return !m_running; }

  /**
   * Returns true if there was an error.
   */
  bool error() const
  { return m_error; }

  /**
   * Returns the elapsed time since start. If the process
   * is not @ref finished, subsequent calls will return
   * different values. If it is finished, the value
   * won't change.
   *
   * @return the time in milliseconds
   */
  int elapsed() const;

public slots:
  /**
   * Cancels a running measurement.
   *
   * This function is provided as a slot so that you can make
   * constructs like following to create a time-out mechanism:
   *
   * \code
   *    // time out after 2 seconds
   *	QTimer::singleShot(2000, latencytimer, SLOT(cancel()));
   * \endcode
   */
  void cancel();

signals:
  /**
   * Signals the receipt of an answer.
   *
   * @param msec	the time in milliseconds that was given as result
   */
  void answer(int msec);

private slots:
  void activity();

private:
  int m_port;
  bool m_running;
  bool m_error;
  KLatencyTimerPrivate *d;
};

#endif

