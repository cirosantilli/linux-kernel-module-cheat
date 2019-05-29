def test(self):
    self.sendline('tbreak main')
    self.sendline('continue')

    self.continue_to('rax')
    self.sendline('set $rax = 3')
    self.continue_to('rbx')
    assert self.get_int('$rax') == 3
    assert self.get_int('$rbx') == 2
