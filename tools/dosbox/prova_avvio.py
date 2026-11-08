import subprocess, time, os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import xtasti
QUI = os.path.dirname(os.path.abspath(__file__))
D = os.path.join(QUI, "dosbox-staging-linux-x86_64-0.83.0-7b400", "dosbox")
amb = dict(os.environ, HOME=os.path.join(QUI, "conf_home"), XDG_CONFIG_HOME=os.path.join(QUI, "conf_home"), SDL_AUDIODRIVER="dummy")
p = subprocess.Popen([D, "--noprimaryconf", "--conf", os.path.join(QUI, "base.conf"), "-c", "mount c " + os.path.join(QUI, "c688"), "-c", "c:", "-c", "688"],
                     env=amb, stdout=open(os.path.join(QUI, "dosbox.log"), "w"), stderr=subprocess.STDOUT)
print("pid", p.pid)
try:
    w = xtasti.finestra()
    print("finestra", hex(w))
    time.sleep(10)
    xtasti.tasto(w, "F5", ctrl=True)
    time.sleep(2)
    subprocess.run(["xwd", "-id", str(w), "-out", os.path.join(QUI, "catture", "prova.xwd")])
finally:
    p.terminate()
    p.wait(10)
