-- Rasterbot profile

-- Minimalist node_ and way_functions in order to test source_ and segment_functions

function node_function (node, result)
end

function way_function (way, result)
  local highway = way:get_value_by_key("highway")
  local name = way:get_value_by_key("name")

  if name then
    result.name = name
  end

  result.forward_mode = mode.cycling
  result.backward_mode = mode.cycling

  result.forward_speed = 15
  result.backward_speed = 15
  result.forward_weight_per_meter = result.forward_speed / 3.6
  result.backward_weight_per_meter = result.backward_speed / 3.6
end

function source_function ()
  raster_source = sources:load(
    "../test/rastersource.asc",
    0,    -- lon_min
    0.1,  -- lon_max
    0,    -- lat_min
    0.1,  -- lat_max
    5,    -- nrows
    4     -- ncols
  )
end

function segment_function (source, target, distance, weight)
  local sourceData = sources:query(raster_source, source.lon, source.lat)
  local targetData = sources:query(raster_source, target.lon, target.lat)
  io.write("evaluating segment: " .. sourceData.datum .. " " .. targetData.datum .. "\n")
  local invalid = sourceData.invalid_data()
  local scaled_weight = weight

  if sourceData.datum ~= invalid and targetData.datum ~= invalid then
    local slope = math.abs(sourceData.datum - targetData.datum) / distance
    scaled_weight = scaled_weight / (1.0 - (slope * 5.0))
    io.write("   slope: " .. slope .. "\n")
    io.write("   was weight: " .. weight .. "\n")
    io.write("   new weight: " .. scaled_weight .. "\n")
  end
  return scaled_weight
end
