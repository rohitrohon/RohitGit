db.temperatures.aggregate([
  {
    $group: {
      _id: "$date",
      national_avg: { $avg: "$temp.avg_c" }
    }
  },
  {
    $facet: {
      hottest_days: [{ $sort: { national_avg: -1 } }, { $limit: 5 }],
      coldest_days: [{ $sort: { national_avg: 1 } }, { $limit: 5 }]
    }
  }
])
