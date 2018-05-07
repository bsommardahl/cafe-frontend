let saveOrder = (order: Order.orderVm, onFinish: Order.orderVm => unit) : unit => {
  Js.Console.log("orderActions:: Persisting order....");
  switch (order.id) {
  | None =>
    OrderStore.add({
      discounts: order.discounts,
      customerName: order.customerName,
      orderItems: order.orderItems,
      paid: order.paid,
    })
    |> Js.Promise.then_(freshOrder => {
         onFinish(freshOrder |> Order.vmFromExistingOrder);
         Js.Promise.resolve(
           Js.Console.log("orderActions:: Added new order."),
         );
       })
    |> ignore;
    ();
  | Some(_id) =>
    let o: Order.updateOrder = Order.vmToUpdateOrder(order);
    OrderStore.update(o)
    |> Js.Promise.then_(updatedOrder => {
         onFinish(updatedOrder |> Order.vmFromExistingOrder);
         Js.Promise.resolve(
           Js.Console.log("orderActions:: Updated existing order."),
         );
       })
    |> ignore;
    ();
  };
};

let payOrder =
    (
      cashier: Cashier.t,
      order: Order.orderVm,
      onFinish: Order.orderVm => unit,
    )
    : unit => {
  let totals =
    OrderItemCalculation.getTotals(order.discounts, order.orderItems);
  let paidOrder = {
    ...order,
    paid:
      Some({
        on: Date.now(),
        by: cashier.name,
        subTotal: totals.subTotal,
        tax: totals.tax,
        discount: totals.discounts,
        total: totals.total,
      }),
  };
  let stream =
    paidOrder |> Order.fromVm |> WebhookEngine.fireForOrder(BeforeOrderPaid);
  stream
  |> Most.observe((orderFromWebhook: Order.t) =>
       saveOrder(
         orderFromWebhook |> Order.toVm,
         (vm: Order.orderVm) => {
           orderFromWebhook |> WebhookEngine.fireForOrder(OrderPaid) |> ignore;
           onFinish(vm);
         },
       )
     )
  |> ignore;
};

let returnOrder =
    (
      cashier: Cashier.t,
      order: Order.orderVm,
      onFinish: Order.orderVm => unit,
    ) =>
  saveOrder(
    {...order, returned: Some({on: Date.now(), by: cashier.name})},
    (vm: Order.orderVm) => {
      vm
      |> Order.fromVm
      |> WebhookEngine.fireForOrder(OrderReturned)
      |> ignore;
      onFinish(vm);
    },
  );

let removeOrder = (order: Order.orderVm, onFinish: Order.orderVm => unit) =>
  switch (order.id) {
  | None => onFinish(order)
  | Some(id) =>
    OrderStore.remove(id)
    |> Js.Promise.then_(() => {
         onFinish(order);
         Js.Promise.resolve(Js.Console.log("Removed order."));
       })
    |> ignore;
    ();
  };