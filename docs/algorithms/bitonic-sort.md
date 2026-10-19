# Bitonic Sort

## Motivation
Sorting algorithms on the GPU have fundamental different design constraints than those designed for CPUs.
CPUs offer fewer, but much smarter cores than GPUs.
Each CPU core is a fully functional independent unit that can execute totaly independent tasks.
This allows easy parallization of single threaded sorting algorithms onto multiple CPU cores via divide and conquer approaches.<br>

While the GPU offers many more threads (cores) than the CPU, the architecture is heavily restricted.
Each GPU thread must either execute the same operation as all the others in its warp (wave) or idle and wait.
This means to achieve maximum performance on a GPU an algorithm must be designed in a way that is <b>stupidly parallel</b>,
meaning no branching between threads.
Furthermore, balanced workload distribution is important for efficient GPU implementations.
If some threads within a warp finish their work earlier than others, their execution lanes remain idle until the remaining threads finish.
Similarly, synchronization barriers force faster threads within a thread block to wait for slower ones.
Uneven workloads therefore lead to wasted GPU performance.<br>
The interested reader can find a deep dive into modern GPU architecture here (add link to GPU architecture).<br>


Traditional comparison-based sorting algorithms such as Quicksort and Mergesort achieve an average time complexity of $\mathcal{O}(n\log_2n)$,
but their data-dependent control flow and potentially irregular workloads can make efficient GPU parallelization challenging.<br>

<b>Bitonic sort </b> offers a different approach.
As a sorting network, its comparison pattern is independent of the input data, providing predictable workloads.
Although its total computational complexity of $\mathcal{O}(n(\log^2n)^2)$ is worse than that of traditional comparison-based sorting algorithms,
its ideal parallel execution time with $k$ threads is $\mathcal{O}((\frac{n}{k} + 1)(\log_2n)^2)$.
This scaling behaviour combined with the predictable workload makes it particularly suitable for GPUs.<br>

While not the best GPU sorting algorithm, it still performs good enough for most use cases and is a good first step to learn about GPU programming,
as it offers insight into more advanced GPU features, such as shared memory and thread block barriers.


## Sorting Network

Javaescript sorting network generation test:
<!-- Arguments: data-count="32", data-draw-width="1.5", data-gap-width="3", data-figure-height="250" -->
<div data-bitonic-sort-graph data-count="32" data-draw-width="1.5" data-gap-width="3" data-figure-height="250" tabindex="0">
  <p>Enable JavaScript to display the bitonic sorting network.</p>
</div>

Red connections show flip operations; orange connections show disperse operations.