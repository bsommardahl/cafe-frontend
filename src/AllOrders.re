open OrderData;

open Util;

type state = {
  startDate: float,
  endDate: float,
  orders: list(Order.orderVm),
  interval: int,
};

type actions =
  | LoadOrders(list(Order.orderVm))
  | ViewOrder(string);

let component = ReasonReact.reducerComponent("AllOrders");

let loadClosedOrders = (state, send) => {
  let getFloatFromOpt = (fl: option(float)) =>
    switch (fl) {
    | None => 0.0
    | Some(n) => n
    };
  CafeStore.getClosedOrders(state.startDate, state.endDate)
  |> Js.Promise.then_(orders => {
       let vms =
         orders
         |> List.map(OrderConversion.vmFromExistingOrder)
         |> List.fast_sort((a, b) =>
              OrderData.Order.(
                compare(
                  getFloatFromOpt(b.paidOn),
                  getFloatFromOpt(a.paidOn),
                )
              )
            );
       send(LoadOrders(vms));
       Js.Promise.resolve();
     })
  |> ignore;
};

let make = (~goBack, _children) => {
  ...component,
  initialState: () => {
    startDate: Date.oneMonthBefore(Date.now()),
    endDate: Date.now(),
    orders: [],
    interval: (-1),
  },
  didMount: self => {
    loadClosedOrders(self.state, self.send);
    ReasonReact.NoUpdate;
  },
  reducer: (action, state) =>
    switch (action) {
    | LoadOrders(orders) => ReasonReact.Update({...state, orders})
    | ViewOrder(id) =>
      ReasonReact.SideEffects(
        (_self => ReasonReact.Router.push("order?orderId=" ++ id)),
      )
    },
  subscriptions: self => [
    Sub(
      () =>
        Js.Global.setInterval(
          () => loadClosedOrders(self.state, self.send),
          5000,
        ),
      Js.Global.clearInterval,
    ),
  ],
  render: self =>
    <div className="all-orders">
      <div className="header">
        <div className="header-menu">
          <div className="card wide-card quiet-card" onClick=goBack>
            (s("Atras"))
          </div>
        </div>
        <div className="header-options">
          (s("Ordenes de los ultimos 30 dias"))
        </div>
      </div>
      <OrderList
        orders=self.state.orders
        onSelect=(
          order =>
            self.send(
              ViewOrder(
                switch (order.id) {
                | Some(id) => id
                | None => ""
                },
              ),
            )
        )
      />
    </div>,
};