db.temperatures.aggregate([
  {
    $group: {
      _id: "$city",
      overall_avg: { $avg: "$temp.avg_c" }
    }
  },
  { $sort: { overall_avg: -1 } },
  {
    $facet: {
      hottest: [{ $limit: 1 }],
      coldest: [{ $sort: { overall_avg: 1 } }, { $limit: 1 }]
    }
  }
])
