CREATE TABLE users (
  id INT PRM,
  name TEXT
);

CREATE TABLE orders (
  order_id INT PRM,
  user_id INT FRN REF users(id),
  amount INT
);
