db.temperatures.aggregate([
  { $match: { city: "Delhi" } },
  { $sort: { date: 1 } },
  {
    $setWindowFields: {
      partitionBy: "$city",
      sortBy: { date: 1 },
      output: {
        moving_avg_7d: {
          $avg: "$temp.avg_c",
          window: { documents: [-6, 0] }
        }
      }
    }
  },
  { $project: { city: 1, date: 1, avg_c: "$temp.avg_c", moving_avg_7d: 1 } }
])
