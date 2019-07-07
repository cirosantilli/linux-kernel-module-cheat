#!/usr/bin/env ruby
#
# https://cirosantilli.com/linux-kernel-module-cheat#asciidoctor-link-target-up-rb

require 'asciidoctor'
require 'asciidoctor/extensions'

require_relative 'link-target-up.rb'

class LinkTargetGitHub < LinkTargetUp
  named :link
  def target_base
    'https://github.com/cirosantilli/linux-kernel-module-cheat/blob/master/'
  end
end

Asciidoctor::Extensions.register do
  inline_macro LinkTargetGitHub
end
