/*
 * QuteCom, a voice over Internet phone
 * Copyright (C) 2010 Mbdsys
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef OWINTERFACE_H
#define OWINTERFACE_H

#include "util/NonCopyable.h"

/**
 * 接口类
 * @author Tanguy Krotoff
 * \ingroup BASIC_CLASS
 */
namespace qcutil {

    class Interface : NonCopyable {
    public:

        virtual ~Interface() {
        }
    };
}
#endif //OWINTERFACE_H
