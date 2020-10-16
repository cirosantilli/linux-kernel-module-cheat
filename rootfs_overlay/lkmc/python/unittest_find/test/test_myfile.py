import unittest

class SomeTest(unittest.TestCase):
    def test_atest(self):
        """
        this test passes
        """
        self.assertEqual(1, 2)
