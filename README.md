# Docker running command

- First build the image:
  ```
  docker build -t ip-augment:latest .
  ```
- Then run container:
  ```
  docker run -d --name ip-augment -p 6379:6379 ip-augment:latest 
  ```

# Redis module command
- request for ip augmentation: `getip any.ip`
- reload ip data file: `reloadip`

# Return value of  `getip ...`
- In normal situation:
  - The return value of this redis command is an array of 2 elements.
  - The 1st element is a integer which indicate the `city_id` of the requested ip. (If this `city_id` == 0, also the result is not usable. So remember to check this value.)
  - The 2nd element is a string which indicate the detailed `region` info about the requested ip.
- Unusual case:
  - If the requested ip is not exist: return value is `nil`.
  - Or, if the 1st element of the return array is 0.
  - So, first to check whether the return value is `nil`, then check the 1st element whether equal to 0.


# ip2region data
- https://github.com/lionsoul2014/ip2region/tree/master/data