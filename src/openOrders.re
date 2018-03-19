open OrderData;

open Util;

type state = {orders: list(Order.order)};

type action =
  | SelectOrder(Order.order);

let orders = CafeStore.getOpenOrders(CafeStore.retrieveAllOrders());

let component = ReasonReact.reducerComponent("OpenOrders");

let make = _children => {
  ...component,
  initialState: () => {orders: orders},
  reducer: (action, _state) =>
    switch (action) {
    | SelectOrder(order) =>
      ReasonReact.SideEffects(
        (
          _self =>
            switch (order.id) {
            | Some(id) =>
              ReasonReact.Router.push("order?orderId=" ++ string_of_int(id))
            | None => ()
            }
        ),
      )
    },
  render: self => {
    let selectOpenOrder = (order: Order.order) =>
      self.send(SelectOrder(order));
    <div className="active-orders">
      (
        self.state.orders
        |> List.map(o =>
             <OpenOrderCard order=o onSelect=(_event => selectOpenOrder(o)) />
           )
        |> Array.of_list
        |> ReasonReact.arrayToElement
      )
    </div>;
  },
};