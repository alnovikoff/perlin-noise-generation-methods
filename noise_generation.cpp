// Fill gradient function
// glm::vec3 get_gradient(glm::vec3 p)
// {
// 
// }

// Smooth Hermite interpolation
static float interpolate_polynomial(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// Linear interpolation
static float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float perlin_noise_improved(glm::vec3 pos)
{
    // Calculate the integer coordinates of the current cell and the next cell
    glm::ivec3 cellCorner = glm::floor(pos);
    glm::ivec3 nextCellCorner = cellCorner + glm::ivec3(1);

    // Calculate the weights based on the fractional part of the position
    glm::vec3 weights = pos - glm::vec3(cellCorner);

    // Apply polynomial interpolation to the weights
    weights.x = interpolate_polynomial(weights.x);
    weights.y = interpolate_polynomial(weights.y);
    weights.z = interpolate_polynomial(weights.z);

    // Calculate dot products between gradients and position vectors for each corner of the cell
    float dot000 = glm::dot(get_gradient(cellCorner), pos - glm::vec3(cellCorner));
    float dot001 = glm::dot(get_gradient(glm::ivec3(cellCorner.x, cellCorner.y, nextCellCorner.z)), pos - glm::vec3(cellCorner.x, cellCorner.y, nextCellCorner.z));
    float dot010 = glm::dot(get_gradient(glm::ivec3(cellCorner.x, nextCellCorner.y, cellCorner.z)), pos - glm::vec3(cellCorner.x, nextCellCorner.y, cellCorner.z));
    float dot011 = glm::dot(get_gradient(glm::ivec3(cellCorner.x, nextCellCorner.y, nextCellCorner.z)), pos - glm::vec3(cellCorner.x, nextCellCorner.y, nextCellCorner.z));
    float dot100 = glm::dot(get_gradient(glm::ivec3(nextCellCorner.x, cellCorner.y, cellCorner.z)), pos - glm::vec3(nextCellCorner.x, cellCorner.y, cellCorner.z));
    float dot101 = glm::dot(get_gradient(glm::ivec3(nextCellCorner.x, cellCorner.y, nextCellCorner.z)), pos - glm::vec3(nextCellCorner.x, cellCorner.y, nextCellCorner.z));
    float dot110 = glm::dot(get_gradient(glm::ivec3(nextCellCorner.x, nextCellCorner.y, cellCorner.z)), pos - glm::vec3(nextCellCorner.x, nextCellCorner.y, cellCorner.z));
    float dot111 = glm::dot(get_gradient(nextCellCorner), pos - glm::vec3(nextCellCorner));

    // Perform linear interpolation in three dimensions to get the final noise value
    float n00 = lerp(weights.x, dot000, dot100);
    float n01 = lerp(weights.x, dot001, dot101);
    float n10 = lerp(weights.x, dot010, dot110);
    float n11 = lerp(weights.x, dot011, dot111);
    float n0 = lerp(weights.y, n00, n10);
    float n1 = lerp(weights.y, n01, n11);
    float n = lerp(weights.z, n0, n1);
    return n;
}

float fractalsum(glm::vec3 const& pos, SpectralSystesisInfo const& info) {
    // Initialize total to store the accumulated sum of fractal noise
    float total = 0.0f;

    // Initialize frequency and amplitude based on the provided information
    float frequency = info.noise_frequency;
    float amplitude = info.start_amplitude;

    // Iterate through different frequencies until reaching the sampling frequency limit
    while (2 * frequency <= info.sampling_frequency) {
        // Add the contribution of Perlin noise at the current frequency and position
        total += perlin_noise_improved(pos * frequency) * amplitude;

        // Update frequency and amplitude for the next iteration
        frequency *= info.frequency_multiplier;
        amplitude *= info.amplitude_multiplier;
    }

    return total;
}

float turbulence(glm::vec3 const& pos, SpectralSystesisInfo const& info) {
    float total = 0.0f;
    float frequency = info.noise_frequency;
    float amplitude = info.start_amplitude;

    while (2 * frequency <= info.sampling_frequency) {
        total += std::fabs(perlin_noise_improved(pos * frequency)) * amplitude;

        frequency *= info.frequency_multiplier;
        amplitude *= info.amplitude_multiplier;
    }

    return total;
}

float marble(glm::vec3 const& pos, SpectralSystesisInfo const& info) {
    float result = 0.0f;
    float amplitude = info.start_amplitude;
    float frequency = info.noise_frequency;

    while (2 * frequency <= info.sampling_frequency) {
        result += perlin_noise_improved(frequency * pos) * amplitude;

        amplitude *= info.amplitude_multiplier;
        frequency *= info.frequency_multiplier;
    }
    result = std::sin(info.arg0 * (pos.x + result));

    return result;
}

