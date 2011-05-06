/**
 * @file   serial_glove.hpp
 * @author Ugo Cupcic <ugo@shadowrobot.com>
 * @date   Thu May  5 15:30:17 2011
*
* Copyright 2011 Shadow Robot Company Ltd.
*
* This program is free software: you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program.  If not, see <http://www.gnu.org/licenses/>.
*
 * @brief Communicate via the serial port with the Cyberglove.
 *
 *
 * List of available commands, gathered from an old specification:
 * (http://www.cyberglovesystems.com/forum/viewtopic.php?f=14&t=247)
 *   - 'F0' -> stops filter
 *   - 'F1' -> starts filter
 *   - '?F' -> current filter status
 *
 *   - 'G'  -> reads data from the cyberglove once
 *   - 'S'  -> streams data from the cyberglove at given period
 *   - 't 1152 1'  -> specifies period of 100Hz
 *   - '?t' -> get period
 *   - '^C' -> stop streaming ??
 *
 *   - 'L0' -> turns light off
 *   - 'L1' -> turns light on
 *   - '?L' -> light status
 *
 *   - 'u1' -> transmits the status each time:
 *               o Bit 0: cyberglove plugged in
 *               o Bit 1: switch on/off
 *               o Bit 2: light on/off
 *   - 'u0' -> stop transmitting the status
 *
 *   - 'd1' -> transmits the timestamp as well
 *
 *   - '?r' -> right handed or not
 *   - '?n' -> number of sensors
 *   - '?i' -> info about the cyberglove
 *   - '^I' or '^R' -> reinitialize / restart the cyberglove firmware (wait 1-2s)
 *
 *
 *
 * Proposed strategy:
 *   - F,1 : set filtering to 0 (faster, we're going to do oversampling anyway)
 *   - U,1 : transmit the status each time
 *   - T,1152,1: set transmit period to 100Hz
 *   - S : streams data
 *         o If data[2] = 0 (light off), ignore
 *         o Else (light on): read the data (start from 5: 2 first letters are 'G' and ' ',
 *           then we have the status bits)
 */

#ifndef _SERIAL_GLOVE_HPP_
#define _SERIAL_GLOVE_HPP_

#include <cereal_port/CerealPort.h>
#include <boost/smart_ptr.hpp>

#include <boost/function.hpp>

namespace cyberglove_freq
{
  struct CybergloveFreq
  {
    static const std::string fastest;
    static const std::string hundred_hz;
    static const std::string fourtyfive_hz;
    static const std::string ten_hz;
    static const std::string one_hz;
  };
}

namespace cyberglove
{
  class CybergloveSerial
  {
  public:
    CybergloveSerial(std::string serial_port, boost::function<void(std::vector<float>, bool)> callback);
    ~CybergloveSerial();

    int set_filtering(bool value);
    int set_transmit_info(bool value);
    int set_frequency(std::string frequency);

    int start_stream();

    int get_nb_msgs_received();

    static const unsigned short glove_size;

  private:
    boost::shared_ptr<cereal::CerealPort> cereal_port;

    void stream_callback(char* world, int length);

    int nb_msgs_received, glove_pos_index;
    std::vector<float> glove_positions;


    int current_value;

    boost::function<void(std::vector<float>, bool)> callback_function;

    bool light_on, button_on;

    bool no_errors;
  };
}

/* For the emacs weenies in the crowd.
Local Variables:
   c-basic-offset: 2
End:
*/

#endif