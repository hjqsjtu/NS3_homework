## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('wifi-timing-attributes', ['stats', 'internet', 'mobility', 'wifi'])
    obj.source = ['wifi-timing-attributes.cc',
                  'wifi-example-apps.cc']
