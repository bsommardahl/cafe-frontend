open OrderHelper;

open Js.Promise;

type state = {
  allProducts: list(Product.t),
  tags: list(string),
  order: Order.orderVm,
  closedOrder: bool,
};

type action =
  | SelectProduct(Product.t)
  | ProductsLoaded(list(Product.t))
  | RemoveOrderItem(OrderItem.t)
  | LoadOrder(Order.orderVm);

let component = ReasonReact.reducerComponent("SearchModal");

let make = (~onCancel=() => (), ~isOpen=false, ~label: string, _children) => {
  ...component,
  reducer: (action, state) =>
    switch (action) {
    | ProductsLoaded(products) =>
      let tags = Product.getTags(products);
      ReasonReact.Update({...state, tags, allProducts: products});
    | LoadOrder(order) =>
      ReasonReact.Update({
        ...state,
        order,
        closedOrder:
          switch (order.paid) {
          | Some(_) => true
          | None => false
          },
      })
    | RemoveOrderItem(orderItem) =>
      ReasonReact.Update({
        ...state,
        order: {
          ...state.order,
          orderItems:
            state.order.orderItems |> List.filter(i => i !== orderItem),
        },
      })
    | SelectProduct(product) =>
      ReasonReact.Update({
        ...state,
        order: {
          ...state.order,
          orderItems:
            List.concat([
              state.order.orderItems,
              [buildOrderItem(product)],
            ]),
        },
      })
    },
  initialState: () => {
    let queryString = ReasonReact.Router.dangerouslyGetInitialUrl().search;
    let customerName =
      switch (Util.QueryParam.get("customerName", queryString)) {
      | Some(name) => name |> Js.Global.decodeURIComponent
      | None => "order.defaultCustomerName" |> Lang.translate
      };
    {
      allProducts: [],
      tags: [],
      order: buildNewOrder(customerName),
      closedOrder: false,
    };
  },
  didMount: self => {
    ProductStore.getAll()
    |> then_(prods => {
         self.send(ProductsLoaded(prods));
         resolve();
       })
    |> ignore;
    let queryString = ReasonReact.Router.dangerouslyGetInitialUrl().search;
    switch (Util.QueryParam.get("orderId", queryString)) {
    | None => ReasonReact.NoUpdate
    | Some(orderId) =>
      getOrderVm(orderId)
      |> Js.Promise.then_(vm => {
           self.send(LoadOrder(vm));
           Js.Promise.resolve();
         })
      |> ignore;
      ReasonReact.NoUpdate;
    };
  },
  render: self =>
    <BsReactstrap.Modal isOpen className="modal">
      <BsReactstrap.ModalHeader className="modal-header">
        (ReactUtils.sloc(label))
      </BsReactstrap.ModalHeader>
      <BsReactstrap.ModalBody className="modal-content">
        <SkuSearch
          allProducts=self.state.allProducts
          productFound=(p => self.send(SelectProduct(p)))
        />
        <div className="heightDivider" />
        <DisplayProduct
          closed=self.state.closedOrder
          order=self.state.order
          onRemoveItem=(i => self.send(RemoveOrderItem(i)))
        />
      </BsReactstrap.ModalBody>
      <BsReactstrap.ModalFooter className="modal-footer">
        <Button
          local=true
          className="cancel-button-card"
          label="action.cancelModal"
          onClick=((_) => onCancel())
        />
      </BsReactstrap.ModalFooter>
    </BsReactstrap.Modal>,
};