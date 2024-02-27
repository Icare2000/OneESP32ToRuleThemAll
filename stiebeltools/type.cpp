/*
 *  Copyright (C) 2023 Bastian Stahmer, based heavily on the great work of Jürg Müller, CH-5524 (see below)
 *  Copyright (C) 2014 Jürg Müller, CH-5524
 * 
 *  This file is part of ha-stiebel-control.
 *  ha-stiebel-control is free software: : you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation version 3 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see http://www.gnu.org/licenses/ .
 */
#include "type.h"

#include <cstring>
#include <algorithm>

#include "mappings.h"

SimpleVariant GetValueByType(const std::uint16_t value, const Type type)
{
    char buffer[32U];
    switch (type)
    {
        case Type::et_byte:
            return (static_cast<std::uint8_t>(value & 0xFF));
        case Type::et_dec_val:
            return (static_cast<std::int16_t>(value) / 10.0f);
        case Type::et_cent_val:
            return (static_cast<std::int16_t>(value) / 100.0f);
        case Type::et_mil_val:
            return (static_cast<std::int16_t>(value) / 1000.0f);
        case Type::et_little_endian:
            return static_cast<std::uint16_t>((((value >> 8U) & 0xFF) | ((value & 0xff) << 8U)));
        case Type::et_little_bool:
            return (value == 0x0100) ? true : false;
        case Type::et_bool:
            return (value == 0x0001) ? true : false;
        case Type::et_betriebsart:
            {
                const auto it = std::find_if(BetriebsartMappings.begin(), BetriebsartMappings.end(), [value](const BetriebsartMapping& element){ return element.id == value;});
                if (it != BetriebsartMappings.end()) {
                    return std::string(it->name);
                }
                return std::string("Unbekannt");
            }
        case Type::et_zeit:
            sprintf(buffer, "%2.2d:%2.2d", ((value >> 8U) & 0xff), (value & 0xff));
            return std::string(buffer);
        case Type::et_datum:
            sprintf(buffer, "%2.2d.%2.2d.", ((value >> 8U) & 0xff), (value & 0xff));
            return std::string(buffer);
        case Type::et_time_domain:
            if (value & 0x8080) {
                return std::string("xx:xx-xx:xx?");
            }
            sprintf(buffer, "%2.2d:%2.2d-%2.2d:%2.2d",
                    (value >> 8U) / 4U, 15U*((value >> 8U) % 4U),
                    (value & 0xff) / 4U, 15U*(value % 4U));
            return std::string(buffer);
        case Type::et_dev_nr:
            if (value >= 0x80) {
                return std::string("--");
            } else {
                sprintf(buffer, "%d", value + 1);
                return std::string(buffer);
            }
        case Type::et_dev_id:
            sprintf(buffer, "%d-%2.2d", ((value >> 8U) & 0xff), (value & 0xff));
            return std::string(buffer);
        case Type::et_err_nr:
        {
            const auto it = std::find_if(ErrorMappings.begin(), ErrorMappings.end(), [value](const ErrorMapping& element){ return element.id == value;});
            if(it != ErrorMappings.end()) {
                return std::string(it->name);
            } else {
                sprintf(buffer, "ERR %d", value);
                return std::string(buffer);
            }
        }
        // just convert to double and handle the decimals in yaml
        case Type::et_double_val:
        case Type::et_triple_val:
            return value * 1.0;
        case Type::et_default:
        default:
            return value;
    }
}