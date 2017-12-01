#!/usr/bin/env ruby
# coding: utf-8

=begin
 Copyright (C) 2017 Vladislav Mileshkin

 This file is part of QtSemanticNotes.

 QtSemanticNotes is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 QtSemanticNotes is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with QtSemanticNotes. If not, see <http://www.gnu.org/licenses/>.
=end

filename = ARGV[0]

images = []

['16', '22', '32', '48', '64', '128', '256'].each do |size|
    src = "#{filename}.svg"
    dest = "#{filename}_#{size}x#{size}.png"
    command = "rsvg-convert -w #{size} -h #{size} #{src} -o #{dest}"
    puts command
    %x[#{command}]
    images << dest
end

command = "icotool -c -o icon.ico #{images.join(' ')}"
puts command
%x[#{command}]
