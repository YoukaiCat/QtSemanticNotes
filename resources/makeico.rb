#!/usr/bin/env ruby
# coding: utf-8

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
