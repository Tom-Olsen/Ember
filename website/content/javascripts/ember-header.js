class EmberHeader {
    constructor(layout) {
        this.layout = layout;
        this.observer = null;
        this.isCompact = false;

        window.addEventListener('resize', () => this.ObserveHeading());
        document.fonts.ready.then(() => this.ObserveHeading());

        if (typeof document$ !== 'undefined') {
            document$.subscribe(() => this.ObserveHeading());
        } else {
            this.ObserveHeading();
        }
    }

    ObserveHeading() {
        this.observer?.disconnect();
        this.observer = null;
        const heading = document.querySelector('.md-content h1');
        if (!heading) {
            this.UpdateCompact(false);
            return;
        }

        // Use the expanded height, independent of the height being animated.
        const expandedHeightRem = Number(getComputedStyle(this.layout).getPropertyValue('--ember-header-expanded-height-rem'));
        const remSize = parseFloat(getComputedStyle(document.documentElement).fontSize);
        // Re-enter slightly below the boundary to avoid subpixel scroll jitter.
        const boundary = expandedHeightRem * remSize + (this.isCompact ? 4 : 0);
        const observer = new IntersectionObserver((entries) => {
            if (observer !== this.observer) {
                return;
            }
            const entry = entries[entries.length - 1];
            this.UpdateCompact(entry.boundingClientRect.bottom < boundary);
        }, {
            rootMargin: `-${boundary}px 0px 0px 0px`,
            threshold: 0
        });
        this.observer = observer;
        observer.observe(heading);
    }

    UpdateCompact(isCompact) {
        if (this.isCompact === isCompact) {
            return;
        }
        this.isCompact = isCompact;
        this.layout.classList.toggle('ember-header-layout--compact', isCompact);
        this.ObserveHeading();
    }
}

const emberHeaderLayout = document.querySelector('.ember-header-layout');
if (emberHeaderLayout) {
    new EmberHeader(emberHeaderLayout);
}