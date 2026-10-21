from pathlib import Path


def on_env(env, config, files):
    brandPath = Path(__file__).resolve().parent.parent / "brand"
    logoPath = brandPath / "ember-logo.svg"
    env.globals["emberLogoSvg"] = logoPath.read_text(encoding="utf-8")
    env.globals["brandIcons"] = {
        "brand/sparkive-logo": (brandPath / "sparkive-logo.svg").read_text(encoding="utf-8")
    }
    return env