import os
import subprocess
import tempfile
import unittest
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
EXE = ROOT / 'game_test'
class GameTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror','-pedantic','Main.cpp','game.cpp','map.cpp','entities.cpp','-o',str(EXE)],cwd=ROOT,check=True)
    @classmethod
    def tearDownClass(cls):
        EXE.unlink(missing_ok=True)
    def run_game(self,commands='',args=()):
        return subprocess.run([str(EXE),*args],input=commands,text=True,capture_output=True,timeout=5)
    def test_commands_and_quit(self):
        p=self.run_game('north\nsouth\neast\nwest\nlook\nunknown\nquit\n');self.assertEqual(p.returncode,0);self.assertIn('Unknown command',p.stdout);self.assertIn('Goodbye',p.stdout)
    def test_gold_once_and_blocked_turn(self):
        p=self.run_game('east\nwest\neast\nwest\nwest\nstatus\nquit\n');self.assertEqual(p.stdout.count('Gold collected!'),1);self.assertIn('Gold: 1',p.stdout);self.assertIn('Blocked',p.stdout);self.assertIn('[4] Enter command',p.stdout)
    def test_hit_and_flags(self):
        p=self.run_game('set poisoned\nset blind\nclear poisoned\nstatus\nhit 999\nstatus\nquit\n');self.assertIn('States: blind',p.stdout);self.assertIn('HP: 0/100 (0.0%)',p.stdout)
    def test_case_alias_and_parser(self):
        p=self.run_game('GO EAST\nW\nNORTH\nuse\nuse 999999999999999999999999\nuse nope\nquit\n');self.assertIn('Invalid item number',p.stdout);self.assertIn('Blocked',p.stdout)
    def test_reachability(self):
        self.assertIn('Exit reachable',self.run_game('check\nquit\n').stdout)
    def test_monster_growth_and_remove(self):
        p=self.run_game('addmonster\naddmonster\naddmonster\nmonsters\ndelmonster 1\nmonsters\nquit\n');self.assertIn('Monsters: 3 Capacity: 4',p.stdout);self.assertIn('Monsters: 2 Capacity: 4',p.stdout)
    def test_dimensions(self):
        self.assertNotEqual(self.run_game(args=['2','2']).returncode,0)
        self.assertEqual(self.run_game('quit\n',args=['15','15']).returncode,0)
    def test_level_valid(self):
        p=self.run_game('check\nquit\n',args=['--level',str(ROOT/'levels/demo.txt')]);self.assertEqual(p.returncode,0);self.assertIn('Exit reachable',p.stdout)
    def test_bad_files(self):
        with tempfile.TemporaryDirectory() as d:
            for i,content in enumerate(['','###\n#@#\n##\n','#####\n#@xE#\n#####\n','#####\n#@..#\n#####\n']):
                path=Path(d)/f'{i}.txt';path.write_text(content)
                p=self.run_game(args=[str(path)]);self.assertNotEqual(p.returncode,0);self.assertIn('damaged level format',p.stdout)
            p=self.run_game(args=[str(Path(d)/'missing.txt')]);self.assertIn('file not found',p.stdout)
    def test_crlf(self):
        with tempfile.TemporaryDirectory() as d:
            path=Path(d)/'crlf.txt';path.write_bytes(b'#####\r\n#@.E#\r\n#####\r\n');self.assertEqual(self.run_game('quit\n',args=[str(path)]).returncode,0)
    def test_long_command(self):
        p=self.run_game('x'*500+'\nquit\n');self.assertIn('Command too long',p.stdout);self.assertIn('Goodbye',p.stdout)
if __name__=='__main__':unittest.main()
