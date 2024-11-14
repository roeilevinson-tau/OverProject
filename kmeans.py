import sys
import math

def output_results(centroids):
    """Prints the final centroids to the console."""
    for centroid in centroids:
        print(",".join(f"{x:.4f}" for x in centroid))

def parse_arguments():
    """Parses command-line arguments."""
    if len(sys.argv) < 3 or len(sys.argv) > 4:
        raise ValueError("Invalid number of arguments")

    try:
        num_clusters = int(sys.argv[1])
        if num_clusters <= 1:
            raise ValueError("Invalid number of clusters")
    except ValueError:
        raise ValueError("Invalid number of clusters")

    if len(sys.argv) == 3:
        max_iterations = 300
        file_path = sys.argv[2]
    else:
        try:
            max_iterations = int(sys.argv[2])
            if max_iterations <= 1 or max_iterations >= 1000:
                raise ValueError("Invalid maximum iteration")
        except ValueError:
            raise ValueError("Invalid maximum iteration")
        file_path = sys.argv[3]

    return num_clusters, file_path, max_iterations


def euclidean_distance(point1, point2):
    """Calculates the Euclidean distance between two points."""
    return math.sqrt(sum((p1 - p2) ** 2 for p1, p2 in zip(point1, point2)))


def read_data_points(file_path):
    """Reads data points from a file."""
    data_points = []
    with open(file_path, 'r') as file:
        for line in file:
            if line.strip():
                data_points.append([float(value) for value in line.split(',')])
    return data_points

def initialize_centroids(data_points, num_clusters):
    """Initializes centroids for the k-means algorithm."""
    if num_clusters >= len(data_points):
        raise ValueError("Invalid number of clusters!")
    return data_points[:num_clusters]


def assign_points_to_centroids(points, centroids):
    """Assigns each point to the closest centroid."""
    assignments = []
    for point in points:
        min_dist = float('inf')
        closest_centroid = 0
        for idx, centroid in enumerate(centroids):
            dist = euclidean_distance(point, centroid)
            if dist < min_dist:
                min_dist = dist
                closest_centroid = idx
        assignments.append(closest_centroid)
    return assignments

def update_centroids(points, assignments, num_clusters, num_dimensions):
    """Updates centroids based on the current assignments."""
    new_centroids = [[0] * num_dimensions for _ in range(num_clusters)]
    counts = [0] * num_clusters

    for point, assignment in zip(points, assignments):
        counts[assignment] += 1
        for dim in range(num_dimensions):
            new_centroids[assignment][dim] += point[dim]

    for idx in range(num_clusters):
        if counts[idx] != 0:
            new_centroids[idx] = [x / counts[idx] for x in new_centroids[idx]]

    return new_centroids

def has_converged(centroids, new_centroids, epsilon):
    """Checks if the centroids have converged within the epsilon."""
    for c, nc in zip(centroids, new_centroids):
        if euclidean_distance(c, nc) >= epsilon:
            return False
    return True

def kmeans(num_clusters, file_path, max_iterations=200, epsilon=0.001):
    """Performs k-means clustering on the data."""
    data_points = read_data_points(file_path)
    num_dimensions = len(data_points[0])
    centroids = initialize_centroids(data_points, num_clusters)

    for iteration in range(max_iterations):
        assignments = assign_points_to_centroids(data_points, centroids)
        new_centroids = update_centroids(data_points, assignments, num_clusters, num_dimensions)

        if has_converged(centroids, new_centroids, epsilon):
            break

        centroids = new_centroids

    output_results(centroids)



def main():
    """Main function to run the k-means clustering algorithm."""
    try:
        num_clusters, file_path, max_iterations = parse_arguments()
        kmeans(num_clusters, file_path, max_iterations)
    except Exception as e:
        print("An Error Has Occurred")

if __name__ == "__main__":
    main()
