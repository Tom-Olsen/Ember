class SsrComparison {
    constructor(root) {
        this.root = root;
        this.viewport = root.querySelector('.ssr-comparison__viewport');
        this.slider = root.querySelector('.ssr-comparison__slider');
        this.rightImage = root.querySelector('[data-ssr-image]');
        this.rightName = root.querySelector('[data-ssr-name]');
        this.rightFps = root.querySelector('[data-ssr-fps]');
        this.descriptions = root.querySelectorAll('[data-ssr-description]');
        this.modeInputs = root.querySelectorAll('input[type="radio"]');
        this.fullscreenButton = root.querySelector('[data-ssr-fullscreen]');
        this.fullscreenLabel = root.querySelector('[data-ssr-fullscreen-label]');
        this.status = root.querySelector('[data-ssr-status]');

        this.slider.addEventListener('input', () => this.UpdatePosition());
        this.viewport.addEventListener('pointerdown', (event) => {
            if (!event.isPrimary || event.button !== 0) {
                return;
            }
            event.preventDefault();
            this.slider.focus({ preventScroll: true });
            this.viewport.setPointerCapture(event.pointerId);
            this.UpdateFromPointer(event);
        });
        this.viewport.addEventListener('pointermove', (event) => {
            if (this.viewport.hasPointerCapture(event.pointerId)) {
                this.UpdateFromPointer(event);
            }
        });
        this.viewport.addEventListener('pointerup', (event) => {
            if (this.viewport.hasPointerCapture(event.pointerId)) {
                this.UpdateFromPointer(event);
                this.viewport.releasePointerCapture(event.pointerId);
            }
        });
        this.modeInputs.forEach((input) => {
            input.addEventListener('change', () => this.SelectMode(input));
        });
        this.fullscreenButton.hidden = !document.fullscreenEnabled;
        this.fullscreenButton.addEventListener('click', () => this.ToggleFullscreen());
        this.root.addEventListener('fullscreenchange', () => this.UpdateFullscreen());
        this.SelectMode(root.querySelector('input[type="radio"]:checked'));
    }

    UpdatePosition() {
        const position = Number(this.slider.value);
        this.root.style.setProperty('--ssr-position', `${position}%`);
        this.slider.setAttribute('aria-valuetext', `${position}% SSR off, ${Number((100 - position).toFixed(1))}% ${this.rightName.textContent}`);
    }

    UpdateFromPointer(event) {
        const bounds = this.slider.getBoundingClientRect();
        const position = Math.max(0, Math.min(100, (event.clientX - bounds.left) / bounds.width * 100));
        this.slider.value = position.toFixed(1);
        this.UpdatePosition();
    }

    SelectMode(input) {
        if (!input.checked) {
            return;
        }
        this.rightImage.src = input.dataset.image;
        this.rightImage.alt = `Scene with ${input.dataset.name.toLowerCase()} screen-space reflections enabled`;
        this.rightName.textContent = input.dataset.name;
        this.rightFps.textContent = input.dataset.fps;
        this.descriptions.forEach((description) => {
            const isSelected = description.dataset.ssrDescription === input.value;
            description.classList.toggle('ssr-comparison__description--active', isSelected);
            description.setAttribute('aria-hidden', String(!isSelected));
        });
        this.UpdatePosition();
    }

    async ToggleFullscreen() {
        this.status.hidden = true;
        try {
            if (document.fullscreenElement === this.root) {
                await document.exitFullscreen();
            } else {
                await this.root.requestFullscreen();
            }
        } catch (error) {
            this.status.textContent = 'Fullscreen could not be opened. Please try again.';
            this.status.hidden = false;
            console.warn('SSR comparison fullscreen request failed.', error);
        }
    }

    UpdateFullscreen() {
        const isFullscreen = document.fullscreenElement === this.root;
        const label = isFullscreen ? 'Exit fullscreen' : 'Enter fullscreen';
        this.fullscreenButton.setAttribute('aria-pressed', String(isFullscreen));
        this.fullscreenButton.setAttribute('aria-label', label);
        this.fullscreenButton.title = isFullscreen ? 'Exit fullscreen (Esc)' : label;
        this.fullscreenLabel.textContent = isFullscreen ? 'Exit fullscreen' : 'Fullscreen';
        if (isFullscreen) {
            this.slider.focus({ preventScroll: true });
        } else {
            this.fullscreenButton.focus({ preventScroll: true });
        }
    }
}

function InitializeSsrComparisons() {
    document.querySelectorAll('[data-ssr-comparison]').forEach((root) => {
        if (root.dataset.initialized !== 'true') {
            new SsrComparison(root);
            root.dataset.initialized = 'true';
        }
    });
}

if (typeof document$ !== 'undefined') {
    document$.subscribe(InitializeSsrComparisons);
} else if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', InitializeSsrComparisons);
} else {
    InitializeSsrComparisons();
}