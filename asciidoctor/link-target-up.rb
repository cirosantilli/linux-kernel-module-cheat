#!/usr/bin/env ruby

# https://cirosantilli.com/linux-kernel-module-cheat#asciidoctor-link-target-up-rb

require 'asciidoctor'
require 'asciidoctor/extensions'

class LinkTargetUp < Asciidoctor::Extensions::InlineMacroProcessor
  use_dsl
  named :link
  ExternalLinkRegex = /^https?:\/\//

  def target_base
    '..'
  end

  def process parent, target, attrs
    text = attrs[1]
    if text.nil? || text.empty?
      text = target
    end
    if !ExternalLinkRegex.match?(target)
      target = File.join(target_base, target)
    end
    create_anchor parent, text, type: :link, target: target
  end
end

Asciidoctor::Extensions.register do
  inline_macro LinkTargetUp
end
