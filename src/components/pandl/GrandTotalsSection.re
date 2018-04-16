open ReactUtils;

let component = ReasonReact.statelessComponent("GrandTotalsSection");

let make =
    (~sales: list(Sale.t), ~expenses: list(Expense.t), ~key="", _children) => {
  ...component,
  render: _self => {
    let groups: list(ProductGroup.t) = sales |> ProductGroup.fromSalesList;
    let salesSubTotal = groups |> ProductGroup.sum(c => c.subTotal);
    let salesTax = groups |> ProductGroup.sum(c => c.tax);
    let salesTotal = groups |> ProductGroup.sum(c => c.total);
    let expensesDenorm = expenses |> Expense.denormalize;
    let expenseSubTotal = expensesDenorm |> Expense.sum(e => e.subTotal);
    let expenseTax = expensesDenorm |> Expense.sum(e => e.tax);
    let expenseTotal = expensesDenorm |> Expense.sum(e => e.total);
    <div className="report-section" key>
      <h4> (s("Totales al Final")) </h4>
      <table className="table">
        <tr>
          <th />
          <th className="number-cell"> (s("Ventas")) </th>
          <th className="number-cell"> (s("Gastos")) </th>
          <th className="number-cell"> (s("Final")) </th>
        </tr>
        <tr>
          <th className="number-cell"> (s("SubTotal")) </th>
          <td className="number-cell">
            (s(salesSubTotal |> Money.toDisplay))
          </td>
          <td className="number-cell">
            (s(expenseSubTotal |> Money.toDisplay))
          </td>
          <td className="number-cell">
            (s(salesSubTotal - expenseSubTotal |> Money.toDisplay))
          </td>
        </tr>
        <tr>
          <th className="number-cell"> (s("Impuesto")) </th>
          <td className="number-cell"> (s(salesTax |> Money.toDisplay)) </td>
          <td className="number-cell">
            (s(expenseTax |> Money.toDisplay))
          </td>
          <td className="number-cell">
            (s(salesTax - expenseTax |> Money.toDisplay))
          </td>
        </tr>
        <tr>
          <th className="number-cell"> (s("Total")) </th>
          <td className="number-cell">
            (s(salesTotal |> Money.toDisplay))
          </td>
          <td className="number-cell">
            (s(expenseTotal |> Money.toDisplay))
          </td>
          <td className="number-cell">
            (s(salesTotal - expenseTotal |> Money.toDisplay))
          </td>
        </tr>
      </table>
    </div>;
  },
};