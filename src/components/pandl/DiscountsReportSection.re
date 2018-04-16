open ReactUtils;

let tableHeader =
  <tr className="table-header">
    <th className="text-cell"> (s("Descuento")) </th>
    <th className="text-cell"> (s("Porcentaje")) </th>
    <th className="number-cell"> (s("Monto")) </th>
  </tr>;

let discountRow = (discount: DiscountGroup.t) =>
  <tr>
    <td className="text-cell"> (s(discount.name)) </td>
    <td className="text-cell">
      (s(discount.percent |> Percent.toDisplay))
    </td>
    <td className="number-cell"> (s(discount.amount |> Money.toDisplay)) </td>
  </tr>;

let component = ReasonReact.statelessComponent("DiscountsReportSection");

let make = (~orders: list(Order.t), ~key="", _children) => {
  ...component,
  render: _self =>
    <div className="report-section" key>
      <table className="table">
        <tbody>
          tableHeader
          (
            orders
            |> DiscountGroup.fromOrders
            |> List.map(g => discountRow(g))
            |> Array.of_list
            |> ReasonReact.arrayToElement
          )
        </tbody>
      </table>
    </div>,
};