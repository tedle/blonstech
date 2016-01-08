# Generates weights & offsets for gaussian blur shader
import argparse
from math import factorial as fact

# Calculates the binomial coefficient for a given distribution's index
def binomial(num_samples, index):
    return fact(num_samples) / (fact(index) * fact(num_samples - index))

# num_samples: Number of texture fetches shader will perform
# linear_sampling: True if texture fetches are expected to sample between pixels
# trim: True if the lowest 2 samples should be excluded to save GPU cycles
def gaussian_weights(num_samples, linear_sampling = False, trim = False):
    num_samples = int(num_samples)

    # Samples dont have to be odd, but its a good idea
    if num_samples % 2 == 0:
        raise ValueError("Must supply an odd number of samples")
    if num_samples < 0:
        raise ValueError("Must supply a positive number of samples")

    # Generate twice as many samples (excluding center)
    if linear_sampling is True:
        num_samples = (num_samples - 1) * 2
        num_samples += 1

    if trim is True:
        num_samples += 4

    # Generate a binomial distribution since its identical to sampled gaussian
    dist = list()
    n = num_samples - 1
    for k in range(n + 1):
        dist += [binomial(n, k)]

    # Trim the lowest 2 values since they aren't worth the GPU cycles
    if trim is True:
        dist = dist[2:-2]

    # Normalize & bisect distribution
    # Doing this after trim prevents image darkening on each pass
    dist = [float(x) / sum(dist) for x in dist[:len(dist) / 2 + 1]]

    # Generate finalized weights
    weights = list()
    # Generate texture sampling coordinate offsets (in pixels)
    offsets = list()
    half_dist_length = (len(dist) - 1) / 2
    if linear_sampling is True:
        # Reduce linear sample weights from pairs into weighted averages
        for i in range(half_dist_length):
            distance = half_dist_length - i
            sample_pair = (dist[i * 2], dist[i * 2 + 1])
            offset_pair = (distance * 2, distance * 2 - 1)
            # Stronger weight gets the larger part of a linear texture fetch
            weighted_offset =                          \
                (sample_pair[0] * offset_pair[0]       \
                    + sample_pair[1] * offset_pair[1]) \
                / sum(sample_pair)
            weights += [sum(sample_pair)]
            offsets += [weighted_offset]
        # Center weight is left as is
        weights += [dist[-1]]
        offsets += [0.0]
    else:
        weights = dist
        offsets = [float(x) for x in range(len(dist) / 2 + 1, -1, -1)]
    
    # Reverse so main sample is at 0 index
    weights = weights[::-1]
    offsets = offsets[::-1]
    print "weights = {}".format(weights)
    print "offsets = {}".format(offsets)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate weights & offsets " +
                                                 "for a gaussian blur shader")
    parser.add_argument("samples",
                        type=int,
                        help="Number of texture fetches to be made by shader")
    parser.add_argument("-t", "--trim",
                        action="store_true",
                        help="Trim the 2 weakest samples to save GPU fetches")
    parser.add_argument("-l", "--linear",
                        action="store_true",
                        help="Blend weights and offsets to " +
                             "utilize linear sampling on GPU")
    args = parser.parse_args()
    gaussian_weights(args.samples, linear_sampling=args.linear, trim=args.trim)
