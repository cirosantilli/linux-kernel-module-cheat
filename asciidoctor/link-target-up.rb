#!/usr/bin/env ruby

=begin
README.html links break because we place that output file in out/
This extension hacks local link targets to the right path.
=end

require 'asciidoctor'
require 'asciidoctor/extensions'

class Main < Asciidoctor::Extensions::InlineMacroProcessor
  use_dsl
  named :link
  ExternalLinkRegex = /^https?:\/\//
  def process parent, target, attrs
    text = attrs[1]
    if text.nil? || text.empty?
      text = target
    end
    if !ExternalLinkRegex.match?(target)
      target = File.join('..', target)
    end
    create_anchor parent, text, type: :link, target: target
  end
end

Asciidoctor::Extensions.register do
  inline_macro Main
end

#(Asciidoctor.load_file(ARGV[0])).convert
