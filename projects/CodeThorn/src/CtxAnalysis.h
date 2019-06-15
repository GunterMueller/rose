#ifndef CTX_ANALYSIS_H
#define CTX_ANALYSIS_H 1

#include "CtxLattice.h"
#include "CtxTransfer.h"
#include "CtxAttribute.h"

namespace CodeThorn
{

//! implements the Decorator pattern to enhance the
//!   PropertyStateFactory with context specific functionality
template <class CallContext>
struct CtxPropertyStateFactory : PropertyStateFactory
{
    typedef CallContext           context_t;
    typedef CtxLattice<context_t> context_lattice_t;

    explicit
    CtxPropertyStateFactory(CodeThorn::PropertyStateFactory& compFac)
    : compFactory(compFac)
    {}

    context_lattice_t* create() ROSE_OVERRIDE
    {
      return new context_lattice_t(compFactory);
    }

    PropertyStateFactory& componentFactory()
    {
      return compFactory;
    }

  private:
    PropertyStateFactory& compFactory;
};


//! analysis class that wraps a context-sensitive analysis around
//!   a non-context-sensitive forward analysis.
template <class CallContext>
struct CtxAnalysis : DFAnalysisBase
{
    typedef DFAnalysisBase        base;
    typedef CallContext           context_t;
    typedef CtxLattice<context_t> context_lattice_t;

    //
    CtxAnalysis( DFAnalysisBase&       compAnalysis,
                 DFTransferFunctions&  compTransfer,
                 PropertyStateFactory& compFactory
               )
    : base(),
      subAnalysis(compAnalysis),
      ctxTransfer(compTransfer, *this),
      ctxFactory(compFactory)
    {
      _initialElementFactory = &ctxFactory;
      _transferFunctions     = &ctxTransfer;
    }

    void initializeExtremalValue(Lattice* element) ROSE_OVERRIDE
    {
      ROSE_ASSERT(element);

      // the initial extremalValue will be set to an empty lattice
      context_lattice_t& lat = dynamic_cast<context_lattice_t&>(*element);
      Lattice*           sub = lat.componentFactory().create();

      ROSE_ASSERT(lat.isBot());
      subAnalysis.initializeExtremalValue(sub);
      lat[context_t()] = sub;
    }

    void initialize(SgProject* root) ROSE_OVERRIDE
    {
      base::initialize(root, true);

      // \todo do we need to initialize component?
      subAnalysis.initialize(root);
    }

    //! retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(Labeler& labeler, Label lblret)
    {
      ROSE_ASSERT(labeler.isFunctionCallReturnLabel(lblret));

      Label    lblcall = labeler.functionCallLabel(astNode(labeler, lblret));
      Lattice* lattice = _analyzerDataPreInfo.at(lblcall.getId());

      return sg::deref(dynamic_cast<const CtxLattice<CallContext> *>(lattice));
    }

  protected:
    CtxAttribute<CallContext>*
    createDFAstAttribute(Lattice* elem) ROSE_OVERRIDE
    {
      context_lattice_t* lat = dynamic_cast<context_lattice_t*>(elem);

      return new CtxAttribute(subAnalysis, elem);
    }

  private:
    DFAnalysisBase&                    subAnalysis;
    CtxTransfer<context_t>             ctxTransfer;
    CtxPropertyStateFactory<context_t> ctxFactory;
};

} // namespace CodeThorn

#endif /* CTX_ANALYSIS_H */
