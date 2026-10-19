class BitonicSortGraph {
    constructor(root) {
        this.root = root;
        this.dataCount = this.ReadNumber('count', 32);
        this.drawWidth = this.ReadNumber('drawWidth', 1.5);
        this.gapWidth = this.ReadNumber('gapWidth', 3);
        this.figureHeight = this.ReadNumber('figureHeight', 250);
        this.dotRadius = 1.5 * this.drawWidth;
        this.padding = Math.max(12, 2 * this.dotRadius);

        if (!Number.isSafeInteger(this.dataCount) || this.dataCount < 2 || !Number.isInteger(Math.log2(this.dataCount))) {
            throw new Error('dataCount must be a power of two greater than or equal to 2.');
        }

        this.Render();
    }

    ReadNumber(name, fallback) {
        const value = this.root.dataset[name] === undefined ? fallback : Number(this.root.dataset[name]);
        if (!Number.isFinite(value) || value <= 0) {
            throw new Error(`${name} must be a positive finite number.`);
        }
        return value;
    }

    CreateElement(name, attributes = {}) {
        const element = document.createElementNS('http://www.w3.org/2000/svg', name);
        Object.entries(attributes).forEach(([key, value]) => element.setAttribute(key, String(value)));
        return element;
    }

    DrawConnection(index0, index1, x, color, lineColor) {
        const y0 = this.padding + this.figureHeight * index0 / (this.dataCount - 1);
        const y1 = this.padding + this.figureHeight * index1 / (this.dataCount - 1);
        const group = this.CreateElement('g');
        group.append(
            this.CreateElement('line', { x1: x, y1: y0, x2: x, y2: y1, stroke: lineColor, 'stroke-width': this.drawWidth }),
            this.CreateElement('circle', { cx: x, cy: y0, r: this.dotRadius, fill: color }),
            this.CreateElement('circle', { cx: x, cy: y1, r: this.dotRadius, fill: color })
        );
        this.connections.append(group);
        this.maxX = Math.max(this.maxX, x);
    }

    DrawFlip(k, index, x) {
        const q = Math.floor((2 * index) / k) * k;
        const kHalf = k / 2;
        const mod = index % kHalf;
        this.DrawConnection(q + mod, q - mod - 1 + k, x, '#ff0000', '#cc0000');
    }

    DrawDisperse(k, index, x) {
        const q = Math.floor((2 * index) / k) * k;
        const kHalf = k / 2;
        const mod = index % kHalf;
        this.DrawConnection(q + mod, q + mod + kHalf, x, '#ff8000', '#cc6600');
    }

    DrawHorizontalLines(width) {
        const lines = this.CreateElement('g', { stroke: '#cccccc', 'stroke-width': this.drawWidth });
        for (let i = 0; i < this.dataCount; i++) {
            const y = this.padding + this.figureHeight * i / (this.dataCount - 1);
            lines.append(this.CreateElement('line', { x1: 0, y1: y, x2: width, y2: y }));
        }
        return lines;
    }

    DrawBitonicSort(xStart) {
        const blockGap = 12.5;
        for (let i = 0; i < this.dataCount / 2; i++) {
            let x = xStart;
            for (let k = 2; k <= this.dataCount; k *= 2) {
                this.DrawFlip(k, i, x + ((2 * i) % k) * this.gapWidth);
                x += k * this.gapWidth + blockGap;
                for (let kk = k / 2; kk > 1; kk /= 2) {
                    this.DrawDisperse(kk, i, x + ((2 * i) % kk) * this.gapWidth);
                    x += kk * this.gapWidth + blockGap;
                }
            }
        }
    }

    Render() {
        this.connections = this.CreateElement('g');
        this.maxX = this.padding;
        this.DrawBitonicSort(this.padding);

        const width = this.maxX + this.padding;
        const height = this.figureHeight + 2 * this.padding;
        const description = `Bitonic sorting network with ${this.dataCount} inputs. Red connections are flips; orange connections are disperses.`;
        const svg = this.CreateElement('svg', { viewBox: `0 0 ${width} ${height}`, width, height, role: 'img', 'aria-label': description });
        const title = this.CreateElement('title');
        title.textContent = description;
        svg.append(title, this.CreateElement('rect', { width, height, fill: 'white' }), this.DrawHorizontalLines(width), this.connections);
        this.root.replaceChildren(svg);
    }

    static Initialize() {
        document.querySelectorAll('[data-bitonic-sort-graph]').forEach((root) => {
            if (root.dataset.initialized === 'true') {
                return;
            }
            try {
                new BitonicSortGraph(root);
                root.dataset.initialized = 'true';
            } catch (error) {
                root.textContent = `Could not draw the bitonic sorting network: ${error.message}`;
                console.warn('Bitonic sorting network rendering failed.', error);
            }
        });
    }
}

if (typeof document$ !== 'undefined') {
    document$.subscribe(() => BitonicSortGraph.Initialize());
} else if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', () => BitonicSortGraph.Initialize());
} else {
    BitonicSortGraph.Initialize();
}